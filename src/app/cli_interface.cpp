#include "cli_interface.h"
#include "../utils/logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

namespace tarius::app
{

    CLIInterface::CLIInterface()
        : m_controller(std::make_unique<AppController>()), m_running(false)
    {
    }

    CLIInterface::~CLIInterface() = default;

    void CLIInterface::run()
    {
        m_running = true;
        displayWelcome();

        // Start a background thread for checking reminders and scheduled tasks
        std::thread reminderThread([this]()
                                   {
        while (m_running) {
            m_controller->checkReminders();
            std::this_thread::sleep_for(std::chrono::seconds(60)); // Check every minute
        } });

        // Main input loop
        std::string input;
        while (m_running)
        {
            std::cout << "You: ";
            std::getline(std::cin, input);

            if (input == "exit" || input == "quit")
            {
                m_running = false;
                break;
            }
            else if (input == "help")
            {
                displayHelp();
            }
            else
            {
                processCommand(input);
            }
        }

        // Wait for reminder thread to finish
        if (reminderThread.joinable())
        {
            reminderThread.join();
        }
    }

    void CLIInterface::displayWelcome()
    {
        std::cout << "=======================================" << std::endl;
        std::cout << "Welcome to Tarius AI - Your Personal AI Twin" << std::endl;
        std::cout << "Type 'help' for available commands or just start chatting!" << std::endl;
        std::cout << "All conversations are stored locally on your device." << std::endl;
        std::cout << "=======================================" << std::endl;
    }

    void CLIInterface::processCommand(const std::string &input)
    {
        // Process the input and get response from the AI
        std::string response = m_controller->processUserInput(input);
        std::cout << "Tarius: " << response << std::endl;
    }

    void CLIInterface::displayHelp()
    {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help - Display this help message" << std::endl;
        std::cout << "  exit/quit - Exit the application" << std::endl;
        std::cout << std::endl;
        std::cout << "You can also:" << std::endl;
        std::cout << "  - Chat naturally with your AI twin" << std::endl;
        std::cout << "  - Ask to schedule events (e.g., 'Schedule a meeting with Bob tomorrow at 3pm')" << std::endl;
        std::cout << "  - Manage tasks (e.g., 'Remind me to call mom this evening')" << std::endl;
        std::cout << "  - Ask for summaries (e.g., 'Summarize our conversation from yesterday')" << std::endl;
    }

} // namespace tarius::app