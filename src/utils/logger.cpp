#include "logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace tarius::utils
{

    bool Logger::s_initialized = false;

    void Logger::init()
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
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::info);
            console_sink->set_pattern("[%^%l%$] %v");

            // Create file sink (10MB max size, 3 rotated files)
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs/tarius.log", 1024 * 1024 * 10, 3);
            file_sink->set_level(spdlog::level::trace);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

            // Create logger with both sinks
            auto logger = std::make_shared<spdlog::logger>("tarius", spdlog::sinks_init_list{console_sink, file_sink});
            logger->set_level(spdlog::level::trace);
            spdlog::set_default_logger(logger);

            spdlog::info("Logger initialized");
            s_initialized = true;
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

    void Logger::shutdown()
    {
        if (s_initialized)
        {
            spdlog::shutdown();
            s_initialized = false;
        }
    }

} // namespace tarius::utils