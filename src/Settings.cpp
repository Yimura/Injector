#include "Common.hpp"
#include "Logger.hpp"
#include "Settings.hpp"

bool Settings::DeepCompare(nlohmann::json& a, const nlohmann::json& b, const bool compareValues)
{
	bool wasModified = false;

	for (auto& e : b.items())
	{
		if (const auto& key = e.key(); a.count(key) == 0 || (compareValues && a[key] != e.value()))
		{
			a[key] = e.value();

			wasModified = true;
		}
		else if (a[key].is_structured() && e.value().is_structured())
		{
			if (DeepCompare(a[key], e.value(), compareValues))
				wasModified = true;
		}
		else if (!a[key].is_structured() && e.value().is_structured()) {
			a[key] = e.value();

			wasModified = true;
		}
	}

	return wasModified;
}

void Settings::LoadImpl()
{
	m_DefaultOptions = ToJsonImpl();

	std::ifstream file(m_FilePath);

	if (!file.is_open())
	{
		SaveImpl();

		file.open(m_FilePath);
	}

	nlohmann::json j;
	try
	{
		file >> j;
	}
	catch (const std::exception& ex)
	{
		LOG(WARNING) << "Failed to load settings:\n" << ex.what();

		SaveImpl();

		return LoadImpl();
	}

	const bool shouldSave = DeepCompare(j, m_DefaultOptions);

	FromJsonImpl(j);

	if (shouldSave)
	{
		LOG(INFO) << "Updated settings...";

		SaveImpl();
	}
}

void Settings::SaveImpl() const
{
	std::ofstream file(m_FilePath, std::ios::out | std::ios::trunc);
	file << ToJsonImpl().dump(4);
	file.close();
}

void Settings::FromJsonImpl(const nlohmann::json& j)
{
	m_ApplicationMode = (ApplicationMode)j["application_mode"];
	m_InjectionMode = (InjectionDirection)j["injection_direction"];
	m_Target = j["target_application"];

	m_Local.m_File = j["local"]["file"];

	m_WebServer.m_DllProvider = j["web_server"]["dll_provider"];
	m_WebServer.m_File = j["web_server"]["file"];
	m_WebServer.m_VersionMachine = j["web_server"]["version"];
}

nlohmann::json Settings::ToJsonImpl() const
{
	return {
		{ "application_mode", (int)m_ApplicationMode },
		{ "injection_direction", (int)m_InjectionMode },
		{ "target_application", m_Target },
		{ "local",
			{
				{"file", m_Local.m_File }
			},
		},
		{ "web_server",
			{
				{ "dll_provider", m_WebServer.m_DllProvider },
				{ "file", m_WebServer.m_File },
				{ "version", m_WebServer.m_VersionMachine }
			}
		}
	};
}