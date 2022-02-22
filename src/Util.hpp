#pragma once
#include "Common.hpp"

namespace Util
{
	const wchar_t* char_to_wchar(const char* a)
	{
		const size_t size = ::strlen(a) + 1;
		auto* wchar = new wchar_t[size];
		mbstowcs(wchar, a, size);

		return wchar;
	}
}