#pragma once

//==========================================================================
// File:        Connection.hpp
// Project:     VeilExchange
//
// Purpose:
//     Declares the Connection class. A Connection object owns one encrypted
//     TCP connection and is responsible for sending and receiving messages.
//==========================================================================

#include "Message.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace VeilExchange
{
    // Maintains one encrypted connection between two peers.
    class Connection : public std::enable_shared_from_this<Connection>
    {
    
    public:
        // Shorter names for commonly used Boost.Asio types.
        using TCP = boost::asio::ip::tcp;
        using TLSSocket = boost::asio::ssl::stream<TCP::socket>;

        // Called when a complete message has been received.
        using MessageReceived = std::function<void(const Message&)>;

        // Called when the TLS handshake has completed successfully.
        using ConnectionEstablished = std::function<void()>;

        // Called if the peer disconnects or a network error occurs.
        using Disconnection =
            std::function<void(const boost::system::error_code&)>;

        Connection(
            boost::asio::io_context& ioContext,
            boost::asio::ssl::context& tlsContext);

        // Gives Application access to the TCP socket when accepting a peer.
        TCP::socket::lowest_layer_type& socket();

        // Connects to a listening peer and performs the client TLS handshake.
        void connect(const std::string& address, std::uint16_t port);

        // Performs the server TLS handshake after a peer has been accepted.
        void performServerHandshake();

        // Places a message in the queue ready to be sent.
        void send(const Message& message);

        // Closes the underlying network connection.
        void close();

        // Sets the functions that Application wants Connection to call when
        // important networking events occur.
        void setMessageReceived(MessageReceived networkState);
        void setConnectionEstablished(ConnectionEstablished networkState);
        void setDisconnection(Disconnection networkState);

    private:
        // Every transmitted message starts with a five-byte header:
        // byte 0    = message type
        // bytes 1-4 = payload size
        static constexpr std::size_t headerSize_ {5};

        // Reads the next message header.
        void readMessageHeader();

        // Reads the payload described by the current header.
        void readPayload();

        // Converts a Message object into bytes ready for transmission.
        std::vector<std::uint8_t> serialiseMessage(
            const Message& message) const;

        // Sends the message at the front of the outgoing queue.
        void sendNextMessage();

        // Handles a networking error and informs Application.
        void reportError(const boost::system::error_code& error);

        // TLS stream containing the TCP socket.
        TLSSocket tlsSocket_;

        // Converts an address and port into usable TCP endpoints.
        TCP::resolver resolver_;

        // Stores the fixed-size header currently being received.
        std::array<std::uint8_t, headerSize_> storedHeader_ {};

        // Stores the payload currently being received.
        std::vector<std::uint8_t> receivedPayload_ {};

        // Stores the message type read from the current header.
        MessageType receivedMessageType_ {MessageType::Text};

        // Stores serialised messages until they can be sent in order.
        std::deque<std::vector<std::uint8_t>> messagesToSend_ {};

        // Application callback functions.
        MessageReceived messageReceived_ {};
        ConnectionEstablished connectionEstablished_ {};
        Disconnection disconnectionHandler_ {};

        // Prevents the disconnection handler being called more than once.
        bool disconnected_ {false};
    };
}
