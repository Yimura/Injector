#pragma once
#include "Common.hpp"

namespace Util
{
	inline const wchar_t* CharToWchar(const char* a)
	{
		const size_t size = ::strlen(a) + 1;
		auto* wchar = new wchar_t[size];
		mbstowcs(wchar, a, size);

		return wchar;
	}
}