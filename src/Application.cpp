//==========================================================================
// File:        Application.cpp
// Project:     VeilExchange
//
// Purpose:
//     Implements the main application behaviour, including TLS setup,
//     listening, connecting, text messages and file transfers.
//==========================================================================

#include "Application.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace VeilExchange
{
    Application::Application(
        const std::string& certificateFile,
        const std::string& privateKeyFile)
        :
        // Create a TLS context that supports current TLS versions.
        tlsContext_(boost::asio::ssl::context::tls)
    {
        // Disable old and insecure SSL/TLS versions.
        tlsContext_.set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::no_tlsv1 |
            boost::asio::ssl::context::no_tlsv1_1);

        // Load this peer's certificate and matching private key.
        tlsContext_.use_certificate_chain_file(certificateFile);
        tlsContext_.use_private_key_file(
            privateKeyFile,
            boost::asio::ssl::context::pem);

        // This is suitable only for a demonstration using self-signed
        // certificates. The connection is encrypted, but the remote peer's
        // identity is not formally checked.
        tlsContext_.set_verify_mode(boost::asio::ssl::verify_none);
    }

    void Application::listen(std::uint16_t port)
    {
        const TCP::endpoint localEndpoint(TCP::v4(), port);

        acceptor_ = std::make_unique<TCP::acceptor>(ioContext_);

        acceptor_->open(localEndpoint.protocol());
        acceptor_->set_option(TCP::acceptor::reuse_address(true));
        acceptor_->bind(localEndpoint);
        acceptor_->listen();

        connectionSetUp();

        std::cout
            << "Listening for a peer on port "
            << port
            << "...\n";

        // Accept one peer. This is enough for the simple demonstration.
        acceptor_->async_accept(
            connection_->socket(),
            [this](const boost::system::error_code& error)
            {
                if (error)
                {
                    std::cerr
                        << "Unable to accept the connection: "
                        << error.message()
                        << '\n';
                    return;
                }

                std::cout
                    << "Peer connected. Starting encrypted handshake...\n";

                // The demo only supports one connected peer, so stop
                // listening once that peer has been accepted.
                boost::system::error_code ignoredError {};
                acceptor_->close(ignoredError);

                connection_->performServerHandshake();
            });
    }

    void Application::connect(
        const std::string& address,
        std::uint16_t port)
    {
        connectionSetUp();

        std::cout
            << "Connecting to "
            << address
            << ':'
            << port
            << "...\n";

        connection_->connect(address, port);
    }

    void Application::run()
    {
        ioContext_.run();
    }

    void Application::stop()
    {
        if (connection_)
        {
            connection_->close();
        }

        if (acceptor_)
        {
            boost::system::error_code ignoredError {};
            acceptor_->close(ignoredError);
        }

        ioContext_.stop();
    }

    void Application::sendTextMessage(const std::string& text)
    {
        if (!isConnected() || !connection_)
        {
            std::cerr
                << "A peer is not connected yet.\n";
            return;
        }

        connection_->send(createMessage(text));
    }

    bool Application::sendFile(const std::string& filePath)
    {
        if (!isConnected() || !connection_)
        {
            std::cerr
                << "A peer is not connected yet.\n";
            return false;
        }

        const std::filesystem::path path(filePath);

        std::ifstream inputFile(path, std::ios::binary);

        if (!inputFile)
        {
            std::cerr
                << "Unable to open file: "
                << filePath
                << '\n';
            return false;
        }

        // Send the filename before sending its contents. Only the filename is
        // sent; the user's complete local path is not shared.
        connection_->send(
            createFileNameMessage(path.filename().string()));

        // Read and send the file in 16 KiB blocks.
        constexpr std::size_t fileBlockSize = 16U * 1024U;
        std::vector<std::uint8_t> fileBuffer(fileBlockSize);

        while (inputFile)
        {
            inputFile.read(
                reinterpret_cast<char*>(fileBuffer.data()),
                static_cast<std::streamsize>(fileBuffer.size()));

            const std::streamsize numberOfBytesRead = inputFile.gcount();

            if (numberOfBytesRead <= 0)
            {
                break;
            }

            // Copy only the bytes that were actually read during this loop.
            std::vector<std::uint8_t> fileData(
                fileBuffer.begin(),
                fileBuffer.begin() + numberOfBytesRead);

            connection_->send(createFileDataMessage(fileData));
        }

        if (inputFile.bad())
        {
            std::cerr
                << "An error occurred while reading the file.\n";
            return false;
        }

        connection_->send(createFileTransferCompleteMessage());

        std::cout
            << "File queued for sending: "
            << path.filename().string()
            << '\n';

        return true;
    }

    bool Application::isConnected() const noexcept
    {
        return connected_.load();
    }

    void Application::connectionSetUp()
    {
        connection_ = std::make_shared<Connection>(
            ioContext_,
            tlsContext_);

        // Process every complete incoming message in Application.
        connection_->setMessageReceived(
            [this](const Message& message)
            {
                handleMessage(message);
            });

        // Record that the encrypted connection is ready for use.
        connection_->setConnectionEstablished(
            [this]()
            {
                connected_.store(true);

                std::cout
                    << "\nCommands:\n"
                    << "  Type a message and press Enter.\n"
                    << "  /file <path> sends a file.\n"
                    << "  /quit closes the program.\n\n"
                    << "> ";

                std::cout.flush();
            });

        // Record that the peer is no longer connected.
        connection_->setDisconnection(
            [this](const boost::system::error_code&)
            {
                connected_.store(false);

                if (incomingFile_.is_open())
                {
                    incomingFile_.close();
                }

                std::cout
                    << "\nPeer disconnected.\n";
            });
    }

    void Application::handleMessage(const Message& message)
    {
        switch (message.messageType)
        {
            case MessageType::Text:
            {
                std::cout
                    << "\nPeer: "
                    << payloadToString(message)
                    << "\n> ";

                std::cout.flush();
                break;
            }

            case MessageType::FileName:
            {
                beginIncomingFile(payloadToString(message));
                break;
            }

            case MessageType::FileData:
            {
                writeIncomingFilePayload(message.payload);
                break;
            }

            case MessageType::FileTransferred:
            {
                finishIncomingFile();
                break;
            }

            default:
            {
                std::cerr
                    << "An unknown message type was received.\n";
                break;
            }
        }
    }

    void Application::beginIncomingFile(const std::string& fileName)
    {
        // Close an earlier incomplete transfer before starting another.
        if (incomingFile_.is_open())
        {
            incomingFile_.close();
        }

        incomingFileName_ = createSafeFileName(fileName);

        if (incomingFileName_.empty())
        {
            incomingFileName_ = "received_file.bin";
        }
        else
        {
            incomingFileName_ = "received_" + incomingFileName_;
        }

        incomingFile_.open(
            incomingFileName_,
            std::ios::binary | std::ios::trunc);

        if (!incomingFile_)
        {
            std::cerr
                << "Unable to create the incoming file.\n";

            incomingFileName_.clear();
            return;
        }

        std::cout
            << "\nReceiving file: "
            << incomingFileName_
            << '\n';
    }

    void Application::writeIncomingFilePayload(
        const std::vector<std::uint8_t>& data)
    {
        if (!incomingFile_.is_open())
        {
            std::cerr
                << "File data arrived without a valid destination file.\n";
            return;
        }

        incomingFile_.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size()));

        if (!incomingFile_)
        {
            std::cerr
                << "Unable to write the received file data.\n";

            incomingFile_.close();
            incomingFileName_.clear();
        }
    }

    void Application::finishIncomingFile()
    {
        if (!incomingFile_.is_open())
        {
            std::cerr
                << "A file-complete message arrived without a file.\n";
            return;
        }

        incomingFile_.close();

        std::cout
            << "File received successfully: "
            << incomingFileName_
            << "\n> ";

        std::cout.flush();

        incomingFileName_.clear();
    }

    std::string Application::createSafeFileName(
        const std::string& fileName) const
    {
        // filename() removes directory sections such as ../../ from a name
        // supplied by the remote peer.
        return std::filesystem::path(fileName).filename().string();
    }
}
