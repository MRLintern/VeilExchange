//==========================================================================
// File:        main.cpp
// Project:     VeilExchange
//
// Purpose:
//     Provides the terminal interface and starts the application in either
//     listening mode or connecting mode.
//==========================================================================

#include "Application.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

namespace
{
    void showInstructions(const std::string& programName)
    {
        std::cout
            << "Usage:\n\n"
            << "  Listen for a peer:\n"
            << "    " << programName << " listen <port>\n\n"
            << "  Connect to a peer:\n"
            << "    " << programName
            << " connect <address> <port>\n\n"
            << "Examples:\n\n"
            << "  " << programName << " listen 5000\n"
            << "  " << programName
            << " connect 127.0.0.1 5000\n";
    }

    std::uint16_t convertPortNumber(const std::string& portText)
    {
        const int port = std::stoi(portText);

        if (port < 1 ||
            port > std::numeric_limits<std::uint16_t>::max())
        {
            throw std::out_of_range(
                "The port must be between 1 and 65535.");
        }

        return static_cast<std::uint16_t>(port);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 3)
        {
            showInstructions(argv[0]);
            return 1;
        }

        const std::string applicationMode = argv[1];

        // These paths are correct when the program is launched from the
        // project's build directory.
        VeilExchange::Application application(
           "cert.pem",
           "privkey.pem");

        if (applicationMode == "listen")
        {
            if (argc != 3)
            {
                showInstructions(argv[0]);
                return 1;
            }

            application.listen(convertPortNumber(argv[2]));
        }
        else if (applicationMode == "connect")
        {
            if (argc != 4)
            {
                showInstructions(argv[0]);
                return 1;
            }

            const std::string peerAddress = argv[2];
            const std::uint16_t peerPort = convertPortNumber(argv[3]);

            application.connect(peerAddress, peerPort);
        }
        else
        {
            std::cerr
                << "Unknown application mode: "
                << applicationMode
                << '\n';

            showInstructions(argv[0]);
            return 1;
        }

        // Networking runs on a separate thread so the main thread can wait
        // for commands entered by the user.
        std::thread networkThread(
            [&application]()
            {
                application.run();
            });

        std::string userInput {};

        while (std::getline(std::cin, userInput))
        {
            if (userInput == "/quit")
            {
                application.stop();
                break;
            }

            constexpr const char* fileCommand = "/file ";

            // rfind(..., 0) checks whether the input begins with "/file ".
            if (userInput.rfind(fileCommand, 0) == 0)
            {
                const std::string filePath = userInput.substr(6);

                if (filePath.empty())
                {
                    std::cerr
                        << "Usage: /file <path>\n";
                    continue;
                }

                application.sendFile(filePath);
                continue;
            }

            if (!userInput.empty())
            {
                application.sendTextMessage(userInput);
            }
        }

        application.stop();

        if (networkThread.joinable())
        {
            networkThread.join();
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Fatal error: "
            << exception.what()
            << '\n';

        return 1;
    }

    return 0;
}
