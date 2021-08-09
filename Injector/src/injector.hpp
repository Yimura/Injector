#pragma once

namespace injector
{
	class Injector
	{
	public:
		static bool inject(const char* target, const char* file);
	private:
		static int get_process_id(const char* target);
		static bool open_process(const int process_id, HANDLE& process_handle);
		static bool inject_into_process(const HANDLE process_id, const char* file);
	};
}

