#pragma once
#include "Common.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include <Psapi.h>
#include <TlHelp32.h>

namespace Injector
{
	inline void* AllocateDllPath(const HANDLE processHandle, const std::string_view dllFile)
	{
		const size_t size = ::strlen(dllFile.data()) + 1;

		return VirtualAllocEx(processHandle, nullptr, size, MEM_COMMIT, PAGE_EXECUTE);
	}

	inline HANDLE LoadLibraryRemotely(const HANDLE processHandle, void* alloc)
	{
		const auto hMod = GetModuleHandleA("kernel32.dll");
		if (!hMod) return INVALID_HANDLE_VALUE;

		const auto loadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
		return CreateRemoteThread(processHandle, nullptr, 0, loadLibrary, alloc, 0, nullptr);
	}

	inline int GetProcessId(std::string_view processName)
	{
		auto* target = Util::CharToWchar(processName.data());

		const auto hToolHelpSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32W processEntry32{};
		processEntry32.dwSize = sizeof(PROCESSENTRY32W);

		if (hToolHelpSnapshot == INVALID_HANDLE_VALUE)
		{
			LOG(WARNING) << "CreateToolhelp32Snapshot returned INVALID_HANDLE_VALUE, unable to proceed.";
			
			return -1;
		}

		int processId = -1;
		for (
			bool foundProcess = Process32FirstW(hToolHelpSnapshot, &processEntry32);
			foundProcess;
			foundProcess = Process32NextW(hToolHelpSnapshot, &processEntry32))
		{
			if (!wcscmp(processEntry32.szExeFile, target))
			{
				processId = processEntry32.th32ProcessID;

				break;
			}
		}

		CloseHandle(hToolHelpSnapshot);
		return processId;
	}

	inline bool OpenProcess(const int processId, HANDLE& processHandle)
	{
		processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, false, processId);

		return processHandle != INVALID_HANDLE_VALUE;
	}

	inline bool WriteToTargetProcess(HANDLE processHandle, void* alloc, std::string_view dllFile)
	{
		const size_t size = ::strlen(dllFile.data()) + 1;
		return WriteProcessMemory(processHandle, alloc, dllFile.data(), size, nullptr);
	}
};
