#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// Debug logging macros that can be disabled
#ifdef TARIUS_DISABLE_DEBUG_LOGS
// No-op macros when debug logs are disabled
#define LOG_TRACE(...) (void)0
#define LOG_DEBUG(...) (void)0
#define LOG_INFO(...) (void)0

// Keep error reporting even in release mode
#define LOG_WARN(...) tarius::utils::Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...) tarius::utils::Logger::error(__VA_ARGS__)
#define LOG_CRITICAL(...) tarius::utils::Logger::critical(__VA_ARGS__)
#else
// Standard logging macros when debug is enabled
#define LOG_TRACE(...) tarius::utils::Logger::trace(__VA_ARGS__)
#define LOG_DEBUG(...) tarius::utils::Logger::debug(__VA_ARGS__)
#define LOG_INFO(...) tarius::utils::Logger::info(__VA_ARGS__)
#define LOG_WARN(...) tarius::utils::Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...) tarius::utils::Logger::error(__VA_ARGS__)
#define LOG_CRITICAL(...) tarius::utils::Logger::critical(__VA_ARGS__)
#endif

// Define a no-op logger for llama.cpp
#ifdef TARIUS_DISABLE_LLAMA_LOGS
#define LLAMA_LOG_INFO(...) (void)0
#define LLAMA_LOG_WARN(...) (void)0
#define LLAMA_LOG_ERROR(...) (void)0
#else
#define LLAMA_LOG_INFO(...) tarius::utils::Logger::info(__VA_ARGS__)
#define LLAMA_LOG_WARN(...) tarius::utils::Logger::warn(__VA_ARGS__)
#define LLAMA_LOG_ERROR(...) tarius::utils::Logger::error(__VA_ARGS__)
#endif

namespace tarius::utils
{
    class Logger
    {
    public:
        static void init(bool console_debug_output = true);
        static void shutdown();

        // Set console output level dynamically
        static void setConsoleLevel(spdlog::level::level_enum level);

        template <typename... Args>
        static void trace(const char *fmt, const Args &...args)
        {
            spdlog::trace(fmt, args...);
        }

        template <typename... Args>
        static void debug(const char *fmt, const Args &...args)
        {
            spdlog::debug(fmt, args...);
        }

        template <typename... Args>
        static void info(const char *fmt, const Args &...args)
        {
            spdlog::info(fmt, args...);
        }

        template <typename... Args>
        static void warn(const char *fmt, const Args &...args)
        {
            spdlog::warn(fmt, args...);
        }

        template <typename... Args>
        static void error(const char *fmt, const Args &...args)
        {
            spdlog::error(fmt, args...);
        }

        template <typename... Args>
        static void critical(const char *fmt, const Args &...args)
        {
            spdlog::critical(fmt, args...);
        }

    private:
        static bool s_initialized;
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_console_sink;
    };

} // namespace tarius::utils