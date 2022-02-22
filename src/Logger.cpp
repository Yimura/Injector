#include "Common.hpp"
#include "Logger.hpp"

struct LogSink
{
	void Callback(g3::LogMessageMover log)
	{
		Logger::GetInstance().m_ConsoleOut << log.get().toString(LogSink::FormatConsole) << std::flush;
		Logger::GetInstance().m_FileOut << log.get().toString(LogSink::FormatFile) << std::flush;
	}

	static LogColor GetColor(const LEVELS& level)
	{
		switch (level.value)
		{
		case g3::kDebugValue:
			return LogColor::BLUE;
		case g3::kInfoValue:
			return LogColor::GREEN;
		case g3::kWarningValue:
			return LogColor::YELLOW;
		}
		return g3::internal::wasFatal(level) ? LogColor::RED : LogColor::WHITE;
	}

	static std::string FormatConsole(const g3::LogMessage& msg)
	{
		LogColor color = LogSink::GetColor(msg._level);
		std::stringstream out;

		out
			<< "[" << msg.timestamp("%H:%M:%S") << "]"
			<< ADD_COLOR_TO_STREAM(color)
			<< "[" << std::setw(7) << msg.level() << "/"
			<< msg.file() << ":" << msg.line() << "]"
			<< RESET_STREAM_COLOR
			<< ": ";

		return out.str();
	}
	static std::string FormatFile(const g3::LogMessage& msg)
	{
		LogColor color = LogSink::GetColor(msg._level);
		std::stringstream out;

		out
			<< "[" << msg.timestamp("%H:%M:%S") << "]"
			<< "[" << std::setw(7) << msg.level() << "/"
			<< msg.file() << ":" << msg.line() << "]"
			<< ": ";

		return out.str();
	}
};

void Logger::DestroyImpl()
{
	m_Worker->removeAllSinks();
	m_Worker.reset();

	m_ConsoleOut.close();
	m_FileOut.close();
}

void Logger::InitImpl()
{
	m_Worker = g3::LogWorker::createLogWorker();

#ifdef _WIN32
	const auto consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD consoleMode;
	GetConsoleMode(consoleHandle, &consoleMode);

	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	SetConsoleMode(consoleHandle, consoleMode);
#endif

	m_ConsoleOut.open("CONOUT$", std::ios::out | std::ios::app);
	m_FileOut.open("./cout.log", std::ios::out | std::ios::trunc);

	m_Worker->addSink(std::make_unique<LogSink>(), &LogSink::Callback);
	g3::initializeLogging(m_Worker.get());
}
