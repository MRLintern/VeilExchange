#pragma once

//==========================================================================
// File:        Application.hpp
// Project:     VeilExchange
//
// Purpose:
//     Declares the main application class. Application sets up TLS, listens
//     for or connects to one peer, sends messages and files, and handles
//     incoming data.
//==========================================================================

#include "Connection.hpp"
#include "Message.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <atomic>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace VeilExchange
{
    class Application
    {
    public:
        using TCP = boost::asio::ip::tcp;

        // certificateFile is the path to the TLS certificate.
        // privateKeyFile is the path to the certificate's private key.
        Application(
            const std::string& certificateFile,
            const std::string& privateKeyFile);

        // Application owns objects such as sockets and an io_context, so it
        // should not be copied or assigned to another Application object.
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Waits for one peer to connect on the selected port.
        void listen(std::uint16_t port);

        // Connects to a peer that is already listening.
        void connect(const std::string& address, std::uint16_t port);

        // Starts the Boost.Asio event loop.
        void run();

        // Stops the connection and event loop.
        void stop();

        // Sends one ordinary text message.
        void sendTextMessage(const std::string& text);

        // Sends one file in smaller blocks so the whole file does not need
        // to be loaded into memory at once.
        bool sendFile(const std::string& filePath);

        // Returns true when the TLS handshake has completed.
        bool isConnected() const noexcept;

    private:
        // Creates the Connection object and installs its callback functions.
        void connectionSetUp();

        // Decides how an incoming Message should be processed.
        void handleMessage(const Message& message);

        // Opens a local destination file after receiving a filename message.
        void beginIncomingFile(const std::string& fileName);

        // Writes one received block of file data to the destination file.
        void writeIncomingFilePayload(
            const std::vector<std::uint8_t>& data);

        // Closes the destination file when the transfer is complete.
        void finishIncomingFile();

        // Removes any directory information from a received filename.
        std::string createSafeFileName(const std::string& fileName) const;

        // Main Boost.Asio event-processing object.
        boost::asio::io_context ioContext_ {};

        // TLS settings shared by the encrypted connection.
        boost::asio::ssl::context tlsContext_;

        // Used when this program is waiting for another peer.
        std::unique_ptr<TCP::acceptor> acceptor_ {};

        // The single active peer connection used by this demo.
        std::shared_ptr<Connection> connection_ {};

        // Destination stream for a file currently being received.
        std::ofstream incomingFile_ {};

        // Name of the file currently being received.
        std::string incomingFileName_ {};

        // Updated after the TLS handshake succeeds or the peer disconnects.
        std::atomic_bool connected_ {false};
    };
}
