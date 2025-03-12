#include "app/cli_interface.h"
#include "utils/logger.h"
#include "utils/config.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // Initialize logger
    tarius::utils::Logger::init();
    LOG_INFO("Starting Tarius AI...");

    // Load configuration
    tarius::utils::Config config;
    if (!config.load())
    {
        LOG_ERROR("Failed to load configuration. Using defaults.");
    }

    // Create and run CLI interface
    tarius::app::CLIInterface cli;
    cli.run();

    LOG_INFO("Tarius AI shutting down.");
    return 0;
}