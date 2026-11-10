#pragma once

#include <format>
#include <print>
#include <source_location>

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
};

class Logger {
public:
    static void setLevel(LogLevel level) { m_level = level; }

    template <typename... Args>
    static void log(LogLevel level, std::source_location location, std::format_string<Args...> fmt, Args &&...args) {
        if (m_level < level) {
            return;
        }

        std::string_view level_color, level_name;

        switch (level) {
        case LogLevel::Debug:
            level_color = "\033[90m";
            level_name = "DEBUG";
            break;
        case LogLevel::Info:
            level_color = "\033[32m";
            level_name = "INFO";
            break;
        case LogLevel::Warn:
            level_color = "\033[33m";
            level_name = "WARNING";
            break;
        case LogLevel::Error:
            level_color = "\033[31m";
            level_name = "ERROR";
            break;
        }
        std::println("\033[1m{}[{:>7}] \033[95m<{}:{}>\033[0m", level_color, level_name, location.file_name(),
                     location.line());
        std::println(">>> {}", std::format(fmt, std::forward<Args>(args)...));
    }

private:
    inline static LogLevel m_level = LogLevel::Error;
};

#ifndef NDEBUG
#define logDebug(...) Logger::log(LogLevel::Debug, std::source_location::current(), __VA_ARGS__)
#define logInfo(...) Logger::log(LogLevel::Info, std::source_location::current(), __VA_ARGS__)
#define logWarn(...) Logger::log(LogLevel::Warn, std::source_location::current(), __VA_ARGS__)
#define logError(...) Logger::log(LogLevel::Error, std::source_location::current(), __VA_ARGS__)
#else
#define logDebug(...)
#define logInfo(...)
#define logWarn(...) Logger::log(LogLevel::Warn, std::source_location::current(), __VA_ARGS__)
#define logError(...) Logger::log(LogLevel::Error, std::source_location::current(), __VA_ARGS__)
#endif
