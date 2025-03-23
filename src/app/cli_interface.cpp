#include "cli_interface.h"
#include "../utils/logger.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <filesystem>

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
        // load in the default model
        bool success = m_controller->initializeLlamaModel("./models/Dolphin3.0-Llama3.2-1B-Q4_K_M.gguf");

        if (success)
        {
            std::cout << "Tarius: Model loaded successfully! I'm now using the LLaMA model to generate responses." << std::endl;
        }
        else
        {
            std::cout << "Tarius: Failed to load the model. Please check the logs for details." << std::endl;
        }

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
                // Process special commands first, then regular input
                if (!processSpecialCommand(input))
                {
                    processCommand(input);
                }
            }
        }

        // Wait for reminder thread to finish
        if (reminderThread.joinable())
        {
            reminderThread.join();
        }
    }

    bool CLIInterface::processSpecialCommand(const std::string &input)
    {
        // Check for special commands that start with a slash
        if (input.empty() || input[0] != '/')
        {
            return false;
        }

        std::string command = input.substr(1); // Remove the slash
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "load_model")
        {
            std::string modelPath;
            iss >> modelPath;

            if (modelPath.empty())
            {
                std::cout << "Tarius: Please specify a path to the model file." << std::endl;
                std::cout << "Usage: /load_model [path_to_model]" << std::endl;
                return true;
            }

            // Check if the model file exists
            if (!std::filesystem::exists(modelPath))
            {
                std::cout << "Tarius: Model file not found at " << modelPath << std::endl;
                return true;
            }

            std::cout << "Tarius: Loading model from " << modelPath << ". This may take a moment..." << std::endl;
            bool success = m_controller->initializeLlamaModel(modelPath);

            if (success)
            {
                std::cout << "Tarius: Model loaded successfully! I'm now using the LLaMA model to generate responses." << std::endl;
            }
            else
            {
                std::cout << "Tarius: Failed to load the model. Please check the logs for details." << std::endl;
            }

            return true;
        }
        else if (cmd == "model_status")
        {
            bool isInitialized = m_controller->isLlamaModelInitialized();
            if (isInitialized)
            {
                std::cout << "Tarius: LLaMA model is initialized and active." << std::endl;
            }
            else
            {
                std::cout << "Tarius: LLaMA model is not currently active." << std::endl;
            }
            return true;
        }

        return false;
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
        std::cout << "  /load_model [path_to_model] - Load a LLaMA model from the specified path" << std::endl;
        std::cout << "  /model_status - Check if the LLaMA model is active" << std::endl;
        std::cout << std::endl;
        std::cout << "You can also:" << std::endl;
        std::cout << "  - Chat naturally with your AI twin" << std::endl;
        std::cout << "  - Ask to schedule events (e.g., 'Schedule a meeting with Bob tomorrow at 3pm')" << std::endl;
        std::cout << "  - Manage tasks (e.g., 'Remind me to call mom this evening')" << std::endl;
        std::cout << "  - Ask for summaries (e.g., 'Summarize our conversation from yesterday')" << std::endl;
    }

} // namespace tarius::app