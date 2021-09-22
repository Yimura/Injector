#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <Windows.h>

#include "nlohmann/json.hpp"
#include "HTTPRequest.hpp"

#include "util/logger.hpp"
#include "settings.hpp"

namespace injector
{
	using namespace std::chrono_literals;
}