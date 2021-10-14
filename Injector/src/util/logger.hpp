#pragma once
#include <cstring>
#include <iostream>
#include <mutex>

#ifdef _WIN32
#include "Windows.h"
#endif

namespace injector
{
#define LOG_ARGS template <typename ...Args>

    class Logger
    {
        mutable std::mutex mutex;

    public:
        enum class LogLevel {
            Verbose,
            Info,
            Warning,
            Error,
            Critical
        };

        Logger(LogLevel logLevel)
        {
#ifdef _WIN32
            HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

            DWORD console_mode;
            GetConsoleMode(console_handle, &console_mode);

            console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
            SetConsoleMode(console_handle, console_mode);
#endif

            this->m_log_level = logLevel;
        }

        ~Logger()
        {
        }

        LOG_ARGS
            void critical(const char* service, const char* format, Args&& ...args)
        {
            this->log(LogLevel::Critical, service, format, std::forward<Args>(args)...);
        }

        LOG_ARGS
            void error(const char* service, const char* format, Args&& ...args)
        {
            this->log(LogLevel::Error, service, format, std::forward<Args>(args)...);
        }

        LOG_ARGS
            void info(const char* service, const char* format, Args&& ...args)
        {
            this->log(LogLevel::Info, service, format, std::forward<Args>(args)...);
        }

        LOG_ARGS
            void verbose(const char* service, const char* format, Args&& ...args)
        {
            this->log(LogLevel::Verbose, service, format, std::forward<Args>(args)...);
        }

        LOG_ARGS
            void warning(const char* service, const char* format, Args&& ...args)
        {
            this->log(LogLevel::Warning, service, format, std::forward<Args>(args)...);
        }


        void set_log_level(LogLevel level)
        {
            this->m_log_level = level;
        }

    private:
        const char* blue = "\x1b[34m";
        const char* green = "\x1b[32m";
        const char* yellow = "\x1b[33m";
        const char* red = "\x1b[31m";
        const char* reset = "\x1b[0m";

        LogLevel m_log_level;

        LOG_ARGS
            void log(LogLevel level, const char* service, const char* format, Args&& ...args)
        {
            const size_t alloc_size = 8;

            char color[alloc_size];
            char level_string[alloc_size];

            switch (level)
            {
            case LogLevel::Verbose:
                strcpy(color, blue);
                strcpy(level_string, "VERB");

                break;
            case LogLevel::Info:
                strcpy(color, green);
                strcpy(level_string, "INFO");

                break;
            case LogLevel::Warning:
                strcpy(color, yellow);
                strcpy(level_string, "WARN");

                break;
            case LogLevel::Error:
                strcpy(color, red);
                strcpy(level_string, "ERR");

                break;
            case LogLevel::Critical:
                strcpy(color, red);
                strcpy(level_string, "CRIT");

                break;
            }

            size_t size_needed = snprintf(nullptr, 0, format, std::forward<Args>(args)...);
            char* message = (char*)malloc(sizeof(char) * size_needed);
            sprintf(message, format, std::forward<Args>(args)...);

            std::lock_guard<std::mutex> lock(this->mutex);
            std::cout << color << "[" << level_string << "/" << service << "] " << reset << message << std::endl;

            // be a good boy and free memory
            free(message);
        }
    };

    inline Logger* g_log = new Logger(Logger::LogLevel::Verbose);
}