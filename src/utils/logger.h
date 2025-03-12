#pragma once

#include <string>
#include <spdlog/spdlog.h>

// Define logging macros
#define LOG_TRACE(...) tarius::utils::Logger::trace(__VA_ARGS__)
#define LOG_DEBUG(...) tarius::utils::Logger::debug(__VA_ARGS__)
#define LOG_INFO(...) tarius::utils::Logger::info(__VA_ARGS__)
#define LOG_WARN(...) tarius::utils::Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...) tarius::utils::Logger::error(__VA_ARGS__)
#define LOG_CRITICAL(...) tarius::utils::Logger::critical(__VA_ARGS__)

namespace tarius::utils
{

    class Logger
    {
    public:
        static void init();
        static void shutdown();

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
    };

} // namespace tarius::utils