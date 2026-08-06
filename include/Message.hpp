#pragma once

//==========================================================================
// File:        Message.hpp
// Project:     VeilExchange
//
// Purpose:
//     Defines the different message types used by the application and the
//     Message structure that stores data before it is sent across the
//     network.
//==========================================================================

#include <cstdint>
#include <string>
#include <vector>

namespace VeilExchange
{

    // Identifies the type of information carried by a Message object.
    enum class MessageType : std::uint8_t
    {
        // An ordinary text message.
        Text = 1,

        // The name of a file that is about to be transferred.
        FileName = 2,

        // One block of data belonging to the file being transferred.
        FileData = 3,

        // Marks the end of the current file transfer.
        FileTransferred = 4
    };

    // Represents one item of information exchanged between two peers.
    struct Message
    {
        // The type of information stored in the payload.
        MessageType messageType {MessageType::Text};

        // The message contents stored as bytes. Bytes can represent both
        // ordinary text and binary file data.
        std::vector<std::uint8_t> payload {};
    };

    // Creates an ordinary text message.
    Message createMessage(const std::string& text);

    // Creates a message containing the name of a file.
    Message createFileNameMessage(const std::string& fileName);

    // Creates a message containing one block of file data.
    Message createFileDataMessage(const std::vector<std::uint8_t>& data);

    // Creates a message marking the end of a file transfer.
    Message createFileTransferCompleteMessage();

    // Converts a payload containing text into a std::string.
    std::string payloadToString(const Message& message);
}
