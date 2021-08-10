#include "common.hpp"
#include "injector.hpp"
#include <filesystem>
#include <process.h>
#include <TlHelp32.h>
#include <Windows.h>

namespace injector
{
	static std::unique_ptr<wchar_t[]> char_to_wchar(const char* text)
	{
		const size_t size = strlen(text) + 1;
		std::unique_ptr<wchar_t[]> wText = std::unique_ptr<wchar_t[]>(new wchar_t[size]);
		mbstowcs(wText.get(), text, size);

		return wText;
	}

	bool Injector::inject(const char* target, const char* file)
	{
		g_log->verbose("INJECTOR", "Starting injection for %s into %s", file, target);

		char file_path[MAX_PATH];
		if (_fullpath(file_path, file, sizeof(file_path)) == NULL || !std::filesystem::exists(file_path))
		{
			g_log->error("INJECTOR", "File %s given for injection could not be found.", file);

			return false;
		}

		int process_id = Injector::get_process_id(target);
		if (!process_id) return false;

		HANDLE hProc;
		bool success = Injector::open_process(process_id, hProc) && Injector::inject_into_process(hProc, file_path);
		CloseHandle(hProc);

		return success;
	}

	int Injector::get_process_id(const char* target)
	{
		auto wtarget = char_to_wchar(target);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32W procentSnapshot{ 0 };
		procentSnapshot.dwSize = sizeof(PROCESSENTRY32W);

		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			g_log->error("INJECTOR", "Failed to create process handle.");

			return 0;
		}

		int process_id = 0;
		for (bool found_process = Process32FirstW(hSnapshot, &procentSnapshot); found_process; found_process = Process32NextW(hSnapshot, &procentSnapshot))
		{
			if (!wcscmp(procentSnapshot.szExeFile, wtarget.get()))
			{
				process_id = procentSnapshot.th32ProcessID;

				break;
			}
		}

		CloseHandle(hSnapshot);
		if (!process_id)
		{
			g_log->error("INJECTOR", "Unable to find target process %s.", target);

			return 0;
		}

		g_log->verbose("INJECTOR", "Found target process with process id %d", process_id);

		return process_id;
	}

	bool Injector::open_process(const int process_id, HANDLE &process_handle)
	{
		process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id);
		
		if (!process_handle)
		{
			g_log->error("INJECTOR", "Failed to access process ID %d", process_id);

			return false;
		}

		g_log->verbose("INJECTOR", "Got process handle.");

		return true;
	}

	bool Injector::inject_into_process(const HANDLE process_handle, const char* file)
	{
		const size_t dll_path_size = strlen(file) + 1;

		g_log->verbose("INJECTOR", "Allocating DLL path.");
		LPVOID alloc = VirtualAllocEx(process_handle, 0, dll_path_size, MEM_COMMIT, PAGE_EXECUTE);
		if (!alloc)
		{
			g_log->error("INJECTOR", "Failed to allocate path in target process.");

			return false;
		}
		g_log->verbose("INJECTOR", "Successfully allocated DLL path.");

		if (!WriteProcessMemory(process_handle, alloc, file, dll_path_size, 0))
		{
			g_log->error("INJECTOR", "Failed to write dll path to target process.");

			return false;
		}
		g_log->verbose("INJECTOR", "Wrote DLL path to target process.");

		LPTHREAD_START_ROUTINE LoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		HANDLE hThreadReturn = CreateRemoteThread(process_handle, 0, 0, LoadLibraryA, alloc, 0, 0);
		if (!hThreadReturn)
		{
			g_log->error("INJECTOR", "Failed to start remote thread.");

			return false;
		}

		return true;
	}
}