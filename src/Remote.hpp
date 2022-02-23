#pragma once
#include "Common.hpp"
#include "Logger.hpp"

namespace Remote
{
	struct VersionInfo
	{
		const std::string m_Path;
		const std::string m_Version;
		const int m_VersionMachine;
		const bool m_Valid;
	};

	inline VersionInfo GetVersionInfo(const std::string_view dllProvider)
	{
		const auto url = std::format("{}/version.json", dllProvider.data());

		try
		{
			http::Request req(url);
			http::Response res = req.send("GET", "", {}, 2500ms);

			nlohmann::json j = nlohmann::json::parse(res.body.begin(), res.body.end());

			return { j["file"], j["version"], j["version_machine"], true };
		}
		catch (const std::exception&)
		{
			LOG(WARNING) << "Failed to get DLL version info, is the host down?";
		}

		return {};
	}

	inline bool DownloadBinary(const std::string_view fileUrl, const std::filesystem::path& location)
	{
		std::ofstream file(location, std::ios::binary | std::ios::trunc);

		try
		{
			http::Request req(fileUrl.data());
			http::Response res = req.send("GET", "", {}, 2500ms);

			std::ostream_iterator<std::uint8_t> outputIter(file);
			std::ranges::copy(res.body.begin(), res.body.end(), outputIter);
		}
		catch (const std::exception&)
		{
			LOG(INFO) << "Failed to download binary, is the host down?";

			file.close();

			return false;
		}
		file.close();

		return true;
	}
}