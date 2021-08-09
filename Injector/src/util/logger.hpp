#pragma once
#include <cstring>
#include <iostream>

namespace injector
{
    class Logger
    {
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
            HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

            DWORD console_mode;
            GetConsoleMode(console_handle, &console_mode);

            console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
            SetConsoleMode(console_handle, console_mode);

            this->m_log_level = logLevel;
        }

        ~Logger()
        {
        }

        void critical(const char* service, const char* message)
        {
            this->log(LogLevel::Critical, service, message);
        }

        void error(const char* service, const char* message)
        {
            this->log(LogLevel::Error, service, message);
        }

        void info(const char* service, const char* message)
        {
            this->log(LogLevel::Info, service, message);
        }

        void verbose(const char* service, const char* message)
        {
            this->log(LogLevel::Verbose, service, message);
        }

        void warning(const char* service, const char* message)
        {
            this->log(LogLevel::Warning, service, message);
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

        void log(LogLevel level, const char* service, const char* message)
        {
            const uint8_t alloc_size = 16;

            char color[alloc_size];
            char level_string[alloc_size];

            switch (level)
            {
            case LogLevel::Verbose:
                strcpy_s(color, alloc_size, blue);
                strcpy_s(level_string, alloc_size, "VERB");

                break;
            case LogLevel::Info:
                strcpy_s(color, alloc_size, green);
                strcpy_s(level_string, alloc_size, "INFO");

                break;
            case LogLevel::Warning:
                strcpy_s(color, alloc_size, yellow);
                strcpy_s(level_string, alloc_size, "WARN");

                break;
            case LogLevel::Error:
                strcpy_s(color, alloc_size, red);
                strcpy_s(level_string, alloc_size, "ERR");

                break;
            case LogLevel::Critical:
                strcpy_s(color, alloc_size, red);
                strcpy_s(level_string, alloc_size, "CRIT");

                break;
            }

            std::cout << color << "[" << level_string << "/" << service << "] " << reset << message << std::endl;
        }
    };

    inline Logger* g_log = new Logger(Logger::LogLevel::Verbose);
}
