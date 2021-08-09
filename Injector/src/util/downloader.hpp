#pragma once
#include "common.hpp"

namespace injector
{
	struct download {
		inline static void file(const char* file_url, const char* location)
		{
            std::ofstream file;
            file.open(location, std::ios::binary);

            curl::curl_ios<std::ostream> writer(file);
            curl::curl_easy easy(writer);

            easy.add<CURLOPT_URL>(file_url);
            easy.add<CURLOPT_FOLLOWLOCATION>(1L);

            try
            {
                easy.perform();
            }
            catch (curl::curl_easy_exception& e)
            {
                g_log->error("DOWNLOADER", e.what());

                e.print_traceback();
            }

            file.close();
		}
	};
}