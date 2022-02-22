#pragma once
#include "Common.hpp"
#include "Singleton.hpp"

enum class ApplicationMode
{
	WEB_SERVER,
	LOCAL
};

enum class InjectionDirection
{
	ATTACH,
	DETACH
};

class Settings final : public Singleton<Settings>
{
public:
	~Settings() = default;
	Settings(const Settings&) = default;
	Settings(Settings&&) noexcept = default;
	Settings& operator=(const Settings&) = default;
	Settings& operator=(Settings&&) noexcept = default;

	static bool DeepCompare(nlohmann::json& a, const nlohmann::json& b, const bool compareValues = false);
	static Settings& Get() { return GetInstance(); }
	static void Load() { GetInstance().LoadImpl(); }
	static void Save() { GetInstance().SaveImpl(); }

	static void FromJson(const nlohmann::json& j) { GetInstance().FromJsonImpl(j); }
	[[nodiscard]] static nlohmann::json ToJson() { return GetInstance().ToJsonImpl(); }

public:
	ApplicationMode m_ApplicationMode = ApplicationMode::WEB_SERVER;
	InjectionDirection m_InjectionMode = InjectionDirection::ATTACH;
	std::string m_Target = "wordpad.exe";

	struct Local
	{
		std::string m_File = "./some_dll.dll";
	} m_Local;

	struct WebServer
	{
		std::string m_DllProvider = "http://example.com";
		std::string m_File = "latest.dll";
		int m_VersionMachine = 010;
	} m_WebServer;

private:
	Settings() = default;
	
	void LoadImpl();
	void SaveImpl() const;

	void FromJsonImpl(const nlohmann::json& j);
	[[nodiscard]] nlohmann::json ToJsonImpl() const;

private:
	friend class Singleton<Settings>;

	nlohmann::json m_DefaultOptions;

	std::filesystem::path m_FilePath = "./settings.json";
};
