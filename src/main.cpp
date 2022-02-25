#include "Common.hpp"
#include "Injector.hpp"
#include "Logger.hpp"
#include "Remote.hpp"
#include "Settings.hpp"

int main(int argc, const char** argv)
{
	Logger::Init();
	Settings::Load();
	LOG(INFO) << "Loaded settings.";

	std::filesystem::path dllFile;
	const std::string_view targetApplication = g.m_Target;

	switch (g.m_ApplicationMode)
	{
	case ApplicationMode::LOCAL:
		dllFile = g.m_Local.m_File;

		break;
	case ApplicationMode::WEB_SERVER:
		dllFile = g.m_WebServer.m_File;

		const auto versionInfo = Remote::GetVersionInfo(g.m_WebServer.m_DllProvider);
		if (!versionInfo.m_Valid)
		{
			LOG(WARNING) << "Host did not return valid version data, does it have a version.json?";

			return 1;
		}

		if (!std::filesystem::exists(dllFile) || g.m_WebServer.m_VersionMachine < versionInfo.m_VersionMachine)
		{
			if (!Remote::DownloadBinary(
				std::format("{}/{}", g.m_WebServer.m_DllProvider, versionInfo.m_Path),
				dllFile))
			{
				return 1;
			}

			LOG(INFO) << "Downloaded new DLL from remote, new DLL version is " << versionInfo.m_Version;
		}

		g.m_WebServer.m_VersionMachine = versionInfo.m_VersionMachine;
		Settings::Save();
		LOG(INFO) << "Saved new version to disk.";

		break;
	}

	if (!std::filesystem::exists(dllFile))
	{
		LOG(WARNING) << "Unable to find DLL file '" << dllFile << "', make sure it exists.";

		return 1;
	}

	if (!dllFile.is_absolute())
		dllFile = std::filesystem::absolute(dllFile);
	LOG(INFO) << "Starting injection for " << dllFile.filename().string();

	switch (Util::CheckIfFileIsValidDll(dllFile))
	{
	case DllValidityError::ACCESS_FAILURE:
		LOG(WARNING) << "Failed to access DLL on disk.";

		return 1;
	case DllValidityError::TOO_SMALL:
		LOG(WARNING) << "DLL file seems inconceivably small, request to inject ignored.";

		return 1;
	case DllValidityError::ALLOCATION_FAILURE:
		LOG(WARNING) << "Failed to allocate memory when checking DLL file.";

		return 1;
	case DllValidityError::NOT_A_DLL:
		LOG(WARNING) << "The file given does not appear to be a valid DLL.";

		return 1;
	case DllValidityError::INVALID_PLATFORM:
		LOG(WARNING) << "The DLL given did not match the target platform the injector.";

		return 1;
	case DllValidityError::VALID:
		LOG(INFO) << "DLL seems valid, proceeding with injection.";

		break;
	}
	
	const int processId = Injector::GetProcessId(targetApplication.data());
	if (processId == -1)
	{
		LOG(WARNING) << "Unable to find target application " << targetApplication << ", is it running?";

		return 1;
	}
	LOG(INFO) << "Found target application " << targetApplication << " with process id #" << processId;

	HANDLE processHandle;
	if (!Injector::OpenProcess(processId, processHandle))
	{
		LOG(WARNING) << "Failed to open target with necessary rights to inject.";

		return 1;
	}
	
	switch (g.m_InjectionMode)
	{
	case InjectionDirection::ATTACH:
		LOG(INFO) << "Attempting LoadLibrary of DLL...";

		void* alloc;
		if (alloc = Injector::AllocateDllPath(processHandle, dllFile.string()); !alloc)
		{
			LOG(WARNING) << "Failed to allocate DLL file path to target process.";

			return 1;
		}

		if (!Injector::WriteToTargetProcess(processHandle, alloc, dllFile.string()))
		{
			LOG(WARNING) << "Failed to write DLL file path to target process.";

			return 1;
		}

		if (!Injector::LoadLibraryRemotely(processHandle, alloc))
		{
			LOG(WARNING) << "Failed to start LoadLibrary call with remote thread.";

			return 1;
		}
		break;
	}
	LOG(INFO) << "Successfully sent instructions to target process!";
	
	Logger::Destroy();

	return 0;
}
