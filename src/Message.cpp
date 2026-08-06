//==========================================================================
// File:        Message.cpp
// Project:     VeilExchange
//
// Purpose:
//     Implements the helper functions used to create the different messages
//     exchanged by the application.
//==========================================================================

#include "Message.hpp"

namespace VeilExchange
{
    Message createMessage(const std::string& text)
    {
        Message message {};

        message.messageType = MessageType::Text;

        // Copy each character of the text into the byte payload.
        message.payload.assign(text.begin(), text.end());

        return message;
    }

    Message createFileNameMessage(const std::string& fileName)
    {
        Message message {};

        message.messageType = MessageType::FileName;

        // A filename is text, so its characters can be stored as bytes.
        message.payload.assign(fileName.begin(), fileName.end());

        return message;
    }

    Message createFileDataMessage(const std::vector<std::uint8_t>& data)
    {
        Message message {};

        message.messageType = MessageType::FileData;
        message.payload = data;

        return message;
    }

    Message createFileTransferCompleteMessage()
    {
        Message message {};

        message.messageType = MessageType::FileTransferred;

        // No additional data is required for this message type.
        message.payload.clear();

        return message;
    }

    std::string payloadToString(const Message& message)
    {
        return std::string(message.payload.begin(), message.payload.end());
    }
}
