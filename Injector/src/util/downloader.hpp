#pragma once
#include "common.hpp"

namespace injector
{
	struct download {
		inline static bool file(const char* file_url, const char* location)
		{
            std::ofstream file;
            file.open(location, std::ios::binary);

			try
			{
				http::Request req(file_url);
				http::Response res = req.send("GET", "", {}, 2500ms);

				std::ostream_iterator<std::uint8_t> output_iter(file);
				std::copy(res.body.begin(), res.body.end(), output_iter);
			}
			catch (const std::exception&)
			{
				g_log->warning("DOWNLOAD", "Failure while downloading, is the host reachable?");

				file.close();

				return false;
			}

			file.close();

			return true;
		}
	};
}