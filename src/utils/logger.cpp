#include "logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace tarius::utils
{
    bool Logger::s_initialized = false;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::s_console_sink = nullptr;

    void Logger::init(bool console_debug_output)
    {
        if (s_initialized)
        {
            return;
        }

        try
        {
            // Create logs directory if it doesn't exist
            fs::create_directories("logs");

            // Create console sink
            s_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            // Set the initial console level based on the debug flag
            if (console_debug_output)
            {
                s_console_sink->set_level(spdlog::level::info);
            }
            else
            {
                s_console_sink->set_level(spdlog::level::warn); // Only warnings and errors
            }

            s_console_sink->set_pattern("[%^%l%$] %v");

            // Create file sink (10MB max size, 3 rotated files)
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs/tarius.log", 1024 * 1024 * 10, 3);
            file_sink->set_level(spdlog::level::trace);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

            // Create logger with both sinks
            auto logger = std::make_shared<spdlog::logger>("tarius", spdlog::sinks_init_list{s_console_sink, file_sink});
            logger->set_level(spdlog::level::trace);
            spdlog::set_default_logger(logger);

#ifndef TARIUS_DISABLE_DEBUG_LOGS
            spdlog::info("Logger initialized");
#endif

            s_initialized = true;
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

    void Logger::setConsoleLevel(spdlog::level::level_enum level)
    {
        if (s_initialized && s_console_sink)
        {
            s_console_sink->set_level(level);
        }
    }

    void Logger::shutdown()
    {
        if (s_initialized)
        {
            spdlog::shutdown();
            s_initialized = false;
            s_console_sink = nullptr;
        }
    }

} // namespace tarius::utils