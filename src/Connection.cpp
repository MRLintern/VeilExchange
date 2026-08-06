//==========================================================================
// File:        Connection.cpp
// Project:     VeilExchange
//
// Purpose:
//     Implements the encrypted connection between two peers.
//==========================================================================

#include "Connection.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace VeilExchange
{
    Connection::Connection(
        boost::asio::io_context& ioContext,
        boost::asio::ssl::context& tlsContext)
        :
        // The TLS stream contains and protects the TCP socket.
        tlsSocket_(ioContext, tlsContext),

        // The resolver uses the same event-processing object.
        resolver_(ioContext)
    {
    }

    Connection::TCP::socket::lowest_layer_type& Connection::socket()
    {
        // TLS wraps the TCP socket. lowest_layer() provides access to it.
        return tlsSocket_.lowest_layer();
    }

    void Connection::connect(
        const std::string& address,
        std::uint16_t port)
    {
        auto self = shared_from_this();

        // The resolver expects the port as text.
        const std::string portString = std::to_string(port);

        resolver_.async_resolve(
            address,
            portString,
            [self](
                const boost::system::error_code& error,
                const TCP::resolver::results_type& endpoints)
            {
                if (error)
                {
                    self->reportError(error);
                    return;
                }

                // Try the available endpoints until a connection succeeds.
                boost::asio::async_connect(
                    self->tlsSocket_.lowest_layer(),
                    endpoints,
                    [self](
                        const boost::system::error_code& connectionError,
                        const TCP::endpoint&)
                    {
                        if (connectionError)
                        {
                            self->reportError(connectionError);
                            return;
                        }

                        // TCP is connected. The connecting peer now performs
                        // the client side of the TLS handshake.
                        self->tlsSocket_.async_handshake(
                            boost::asio::ssl::stream_base::client,
                            [self](
                                const boost::system::error_code& handshakeError)
                            {
                                if (handshakeError)
                                {
                                    self->reportError(handshakeError);
                                    return;
                                }

                                std::cout
                                    << "Encrypted connection established.\n";

                                if (self->connectionEstablished_)
                                {
                                    self->connectionEstablished_();
                                }

                                // Start waiting for messages from the peer.
                                self->readMessageHeader();
                            });
                    });
            });
    }

    void Connection::performServerHandshake()
    {
        auto self = shared_from_this();

        // The TCP connection has already been accepted. This peer performs
        // the server side of the TLS handshake.
        tlsSocket_.async_handshake(
            boost::asio::ssl::stream_base::server,
            [self](const boost::system::error_code& error)
            {
                if (error)
                {
                    self->reportError(error);
                    return;
                }

                std::cout
                    << "Encrypted connection established.\n";

                if (self->connectionEstablished_)
                {
                    self->connectionEstablished_();
                }

                self->readMessageHeader();
            });
    }

    void Connection::send(const Message& message)
    {
        // Convert the Message object into a single byte vector containing
        // its header followed by its payload.
        std::vector<std::uint8_t> serialisedMessage =
            serialiseMessage(message);

        auto self = shared_from_this();

        // Place all access to the outgoing queue on the Asio event thread.
        boost::asio::post(
            tlsSocket_.get_executor(),
            [self,
             serialisedMessage = std::move(serialisedMessage)]() mutable
            {
                // If the queue is not empty, another write is already active.
                const bool messageCurrentlySending =
                    !self->messagesToSend_.empty();

                self->messagesToSend_.push_back(
                    std::move(serialisedMessage));

                // Start a write only when no earlier message is being sent.
                if (!messageCurrentlySending)
                {
                    self->sendNextMessage();
                }
            });
    }

    void Connection::close()
    {
        auto self = shared_from_this();

        boost::asio::post(
            tlsSocket_.get_executor(),
            [self]()
            {
                boost::system::error_code ignoredError {};

                self->tlsSocket_.lowest_layer().shutdown(
                    TCP::socket::shutdown_both,
                    ignoredError);

                self->tlsSocket_.lowest_layer().close(ignoredError);
            });
    }

    void Connection::setMessageReceived(MessageReceived networkState)
    {
        messageReceived_ = std::move(networkState);
    }

    void Connection::setConnectionEstablished(
        ConnectionEstablished networkState)
    {
        connectionEstablished_ = std::move(networkState);
    }

    void Connection::setDisconnection(Disconnection networkState)
    {
        disconnectionHandler_ = std::move(networkState);
    }

    void Connection::readMessageHeader()
    {
        auto self = shared_from_this();

        // async_read() waits until all five header bytes arrive, unless an
        // error occurs first.
        boost::asio::async_read(
            tlsSocket_,
            boost::asio::buffer(storedHeader_),
            [self](
                const boost::system::error_code& error,
                std::size_t)
            {
                if (error)
                {
                    self->reportError(error);
                    return;
                }

                // The first byte tells us which type of message follows.
                self->receivedMessageType_ =
                    static_cast<MessageType>(self->storedHeader_[0]);

                // Rebuild the 32-bit payload size from four individual bytes.
                const std::uint32_t payloadSize =
                    (static_cast<std::uint32_t>(self->storedHeader_[1]) << 24U) |
                    (static_cast<std::uint32_t>(self->storedHeader_[2]) << 16U) |
                    (static_cast<std::uint32_t>(self->storedHeader_[3]) << 8U) |
                    static_cast<std::uint32_t>(self->storedHeader_[4]);

                // Individual messages are limited to 64 KiB. Larger files
                // are divided into several smaller FileData messages.
                constexpr std::uint32_t maximumPayloadSize =
                    64U * 1024U;

                if (payloadSize > maximumPayloadSize)
                {
                    std::cerr
                        << "The peer sent a payload that is too large.\n";

                    self->close();
                    return;
                }

                self->receivedPayload_.resize(payloadSize);

                // FileTransferred normally has an empty payload, so it can
                // be handled immediately without another read operation.
                if (payloadSize == 0)
                {
                    Message message {};
                    message.messageType = self->receivedMessageType_;

                    if (self->messageReceived_)
                    {
                        self->messageReceived_(message);
                    }

                    self->readMessageHeader();
                    return;
                }

                self->readPayload();
            });
    }

    void Connection::readPayload()
    {
        auto self = shared_from_this();

        boost::asio::async_read(
            tlsSocket_,
            boost::asio::buffer(receivedPayload_),
            [self](
                const boost::system::error_code& error,
                std::size_t)
            {
                if (error)
                {
                    self->reportError(error);
                    return;
                }

                Message message {};

                message.messageType = self->receivedMessageType_;
                message.payload = std::move(self->receivedPayload_);

                if (self->messageReceived_)
                {
                    self->messageReceived_(message);
                }

                // Continue listening for the next message.
                self->readMessageHeader();
            });
    }

    std::vector<std::uint8_t> Connection::serialiseMessage(
        const Message& message) const
    {
        if (message.payload.size() >
            std::numeric_limits<std::uint32_t>::max())
        {
            throw std::length_error("The message payload is too large.");
        }

        const auto payloadSize =
            static_cast<std::uint32_t>(message.payload.size());

        std::vector<std::uint8_t> serialisedMessage {};

        // Reserve enough memory for the header and payload before inserting
        // data. This avoids repeated vector resizing.
        serialisedMessage.reserve(headerSize_ + message.payload.size());

        // Header byte 0: message type.
        serialisedMessage.push_back(
            static_cast<std::uint8_t>(message.messageType));

        // Header bytes 1-4: payload size in network byte order.
        serialisedMessage.push_back(
            static_cast<std::uint8_t>((payloadSize >> 24U) & 0xFFU));
        serialisedMessage.push_back(
            static_cast<std::uint8_t>((payloadSize >> 16U) & 0xFFU));
        serialisedMessage.push_back(
            static_cast<std::uint8_t>((payloadSize >> 8U) & 0xFFU));
        serialisedMessage.push_back(
            static_cast<std::uint8_t>(payloadSize & 0xFFU));

        // Add the payload after the header.
        serialisedMessage.insert(
            serialisedMessage.end(),
            message.payload.begin(),
            message.payload.end());

        return serialisedMessage;
    }

    void Connection::sendNextMessage()
    {
        if (messagesToSend_.empty())
        {
            return;
        }

        auto self = shared_from_this();

        // The vector at the front of the queue remains alive until the
        // asynchronous write has finished using it.
        boost::asio::async_write(
            tlsSocket_,
            boost::asio::buffer(messagesToSend_.front()),
            [self](
                const boost::system::error_code& error,
                std::size_t)
            {
                if (error)
                {
                    self->reportError(error);
                    return;
                }

                // The message was sent, so remove it from the queue.
                self->messagesToSend_.pop_front();

                // Send the next queued message, if there is one.
                if (!self->messagesToSend_.empty())
                {
                    self->sendNextMessage();
                }
            });
    }

    void Connection::reportError(
        const boost::system::error_code& error)
    {
        // A single network failure can complete several operations. Report
        // the disconnection only once.
        if (disconnected_)
        {
            return;
        }

        disconnected_ = true;

        // EOF and operation_aborted commonly occur during normal shutdown.
        if (error != boost::asio::error::eof &&
            error != boost::asio::error::operation_aborted &&
            error != boost::asio::ssl::error::stream_truncated)
        {
            std::cerr
                << "Connection error: "
                << error.message()
                << '\n';
        }

        boost::system::error_code ignoredError {};
        tlsSocket_.lowest_layer().close(ignoredError);

        if (disconnectionHandler_)
        {
            disconnectionHandler_(error);
        }
    }
}
