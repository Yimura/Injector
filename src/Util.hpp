#pragma once
#include "Common.hpp"

enum class DllValidityError
{
	VALID,
	ACCESS_FAILURE,
	TOO_SMALL,
	ALLOCATION_FAILURE,
	NOT_A_DLL,
	INVALID_PLATFORM
};

namespace Util
{
	inline const wchar_t* CharToWchar(const char* a)
	{
		const size_t size = ::strlen(a) + 1;
		auto* wchar = new wchar_t[size];
		mbstowcs(wchar, a, size);

		return wchar;
	}

	inline DllValidityError CheckIfFileIsValidDll(const std::filesystem::path& dllFile)
	{
		std::ifstream fileStream(dllFile, std::ios::binary | std::ios::ate);

		if (fileStream.fail())
		{
			fileStream.close();

			return DllValidityError::ACCESS_FAILURE;
		}

		const auto fileSize = fileStream.tellg();
		if (fileSize < 0x1000)
		{
			fileStream.close();

			return DllValidityError::TOO_SMALL;
		}

		auto* pSrcData = new uint8_t[static_cast<uintptr_t>(fileSize)];
		if (!pSrcData)
		{
			fileStream.close();

			return DllValidityError::ALLOCATION_FAILURE;
		}

		fileStream.seekg(0, std::ios::beg);
		fileStream.read(reinterpret_cast<char*>(pSrcData), fileSize);
		fileStream.close();

		if (reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_magic != 0x5A4D)
		{
			delete[] pSrcData;

			return DllValidityError::NOT_A_DLL;
		}

		const auto* pOldNtHeader	= reinterpret_cast<IMAGE_NT_HEADERS*>(pSrcData + reinterpret_cast<IMAGE_DOS_HEADER*>(pSrcData)->e_lfanew);
		const auto* pOldFileHeader	= &pOldNtHeader->FileHeader;

#ifdef _WIN64
		if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_AMD64)
		{
			delete[] pSrcData;
			delete pOldFileHeader;
			delete pOldNtHeader;

			return DllValidityError::INVALID_PLATFORM;
		}
#else
		if (pOldFileHeader->Machine != IMAGE_FILE_MACHINE_I386)
		{
			delete[] pSrcData;
			delete pOldFileHeader;
			delete pOldNtHeader;

			return DllValidityError::INVALID_PLATFORM;
		}
#endif

		return DllValidityError::VALID;
	}
}