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

			try
			{
				http::Request req(url);
				http::Response res = req.send("GET", "", {}, 2500ms);

				return nlohmann::json::parse(res.body.begin(), res.body.end());
			}
			catch (const std::exception&)
			{
				return nullptr;
			}
		}
	};
}