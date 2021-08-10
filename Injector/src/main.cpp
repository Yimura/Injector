#include "common.hpp"
#include "api/api.hpp"
#include "injector.hpp"
#include "util/downloader.hpp"

int main(int argc, const char** args)
{
	using namespace injector;

	const char* target_app;
	const char* dll_name;

	if (argc == 1)
	{
		g_log->verbose("MAIN", "Loading settings");
		g_settings.load();

		target_app = g_settings.target_application.c_str();
		dll_name = "./latest.dll";
	}
	else if (argc == 3)
	{
		target_app = args[1];
		dll_name = args[2];

		if (Injector::inject(target_app, dll_name))
			g_log->verbose("MAIN", "Successfully injected...");
		else
			g_log->error("MAIN", "Failed to inject into target process.");

		return 0;
	}
	else
	{
		g_log->error("MAIN", "Invalid argument count.");

		return 1;
	}

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

		std::string url = g_settings.dll_provider + "/" + json["file"].get<std::string>();

		g_log->verbose("MAIN", "Downloading file from %s", url.c_str());

		download::file(url.c_str(), dll_name);

		g_log->info("MAIN", "Downloaded DLL, new version %s", json["version"].get<std::string>().c_str());

		g_settings.version = json["version_machine"];
		g_settings.attempt_save();
	}

	if (Injector::inject(target_app, dll_name))
	{
		g_log->verbose("MAIN", "Successfully injected...");

		return 0;
	}
	
	g_log->error("MAIN", "Failed to inject into target process.");

	system("timeout 10");

	return 1;
}