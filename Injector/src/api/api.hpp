#pragma once
#include "common.hpp"

namespace injector
{
	struct api
	{
		static nlohmann::json get_version_info()
		{
			if (g_settings.dll_provider.empty()) return nullptr;

			char url[128];
			sprintf(url, "%s/version.json", g_settings.dll_provider.c_str());

			std::ostringstream stream;
			curl::curl_ios<std::ostringstream> writer(stream);

			curl::curl_easy easy(writer);

			easy.add<CURLOPT_URL>(url);
			easy.add<CURLOPT_FOLLOWLOCATION>(1L);

			try
			{
				easy.perform();

				return nlohmann::json::parse(stream.str());
			}
			catch (const curl::curl_easy_exception& e)
			{
				g_log->error("API", e.what());
			}

			return nullptr;
		}
	};
}