#pragma once

#include <process.h>
#include <Windows.h>

#include <chrono>

#include <filesystem>
#include <fstream>

#include <format>
#include <string_view>

#include <HTTPRequest.hpp>
#include <nlohmann/json.hpp>

#include "Settings.hpp"

inline Settings& g = Settings::Get();

using namespace std::chrono_literals;