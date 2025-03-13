#pragma once

#include "app_controller.h"
#include <string>
#include <memory>
#include <atomic>

namespace tarius::app
{

    class CLIInterface
    {
    public:
        CLIInterface();
        ~CLIInterface();

        void run();

    private:
        void displayWelcome();
        void processCommand(const std::string &input);
        void displayHelp();
        bool processSpecialCommand(const std::string &input); // Returns true if command was processed

        std::unique_ptr<AppController> m_controller;
        std::atomic<bool> m_running;
    };

} // namespace tarius::app