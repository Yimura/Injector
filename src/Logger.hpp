#pragma once
#include "Common.hpp"
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "Singleton.hpp"

#define ADD_COLOR_TO_STREAM(color) "\x1b[" << int(color) << "m"
#define RESET_STREAM_COLOR "\x1b[" << int(LogColor::RESET) << "m"
enum class LogColor
{
	RESET,
	WHITE = 97,
	CYAN = 36,
	MAGENTA = 35,
	BLUE = 34,
	GREEN = 32,
	YELLOW = 33,
	RED = 31,
	BLACK = 30
};

struct LogSink;
class Logger final : public Singleton<Logger>
{
public:
	~Logger() = default;
	Logger(const Logger&) = default;
	Logger(Logger&&) noexcept = default;
	Logger& operator=(const Logger&) = default;
	Logger& operator=(Logger&&) noexcept = default;

	static void Destroy()	{ GetInstance().DestroyImpl(); }
	static void Init()		{ GetInstance().InitImpl(); }

private:
	Logger() = default;

	void DestroyImpl();
	void InitImpl();

private:
	friend class Singleton<Logger>;
	friend struct LogSink;

	std::ofstream m_ConsoleOut;
	std::ofstream m_FileOut;

	std::unique_ptr<g3::LogWorker> m_Worker;
};