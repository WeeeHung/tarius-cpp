#pragma once

#include "app_controller.h"
#include <string>
#include <memory>

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

        std::unique_ptr<AppController> m_controller;
        bool m_running;
    };

} // namespace tarius::app