#include "common.hpp"
#include "api/api.hpp"
#include "injector.hpp"
#include "util/downloader.hpp"

int main()
{
	using namespace injector;
	const char* dll_name = "./YimMenu.dll";

	g_log->verbose("MAIN", "Loading settings");

	g_settings.load();

	if (g_settings.dll_provider.empty())
	{
		g_log->error("MAIN", "DLL Provider is empty in settings, please fill one in.");

		system("timeout 10");

		return 0;
	}

	auto json = api::get_version_info();

	bool dll_exists = std::filesystem::exists(dll_name);
	if (!dll_exists || json["version_machine"] > g_settings.version)
	{
		if (dll_exists)
			g_log->info("MAIN", "DLL is outdated, downloading new version.");
		else
			g_log->info("MAIN", "No DLL present, downloading from server.");

		std::string url = g_settings.dll_provider + "/YimMenu.dll";
		download::file(url.c_str(), dll_name);

		char message[64];
		sprintf(message, "Downloaded DLL, new version %s", json["version"].get<std::string>().c_str());
		g_log->info("MAIN", message);

		g_settings.version = json["version_machine"];
		g_settings.attempt_save();
	}

	if (Injector::inject(g_settings.target_application.c_str(), dll_name))
		g_log->verbose("MAIN", "Successfully injected...");
	else
		g_log->error("MAIN", "Failed to inject into target process.");

	system("timeout 10");

	return 0;
}