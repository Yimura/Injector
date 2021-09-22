workspace "Injector"
	architecture "x64"
	startproject "Injector"

	configurations
	{
		"Debug",
		"Release"
	}

	outputdir = "%{cfg.buildcfg}"

	IncludeDir = {}
	IncludeDir["json"] = "vendor/json/single_include"

	CppVersion = "C++17"
	MsvcToolset = "v142"
	WindowsSdkVersion = "10.0"

	function DeclareMSVCOptions()
		filter "system:windows"
		staticruntime "Off"
		floatingpoint "Fast"
		vectorextensions "AVX2"
		systemversion (WindowsSdkVersion)
		toolset (MsvcToolset)
		cppdialect (CppVersion)

		defines
		{
		  "_CRT_SECURE_NO_WARNINGS",
		  "NOMINMAX",
		  "WIN32_LEAN_AND_MEAN",
		  "_WIN32_WINNT=0x601" -- Support Windows 7
		}

		disablewarnings
		{
			"4068" -- C4068: unknown pragma X
		}

		linkoptions
		{
			"-IGNORE:4286" -- symbol X defined in Y is imported by Z
		}
	end

	function DeclareDebugOptions()
		filter "configurations:Debug"
			defines { "_DEBUG" }
			symbols "On"
		filter "not configurations:Debug"
			defines { "NDEBUG" }
	end

	project "Injector"
		location "%{prj.name}"
		kind "ConsoleApp"
		language "C++"

		targetdir ("bin/" .. outputdir)
		objdir ("bin/int/" .. outputdir .. "/%{prj.name}")

		PrecompileHeaderInclude = "common.hpp"
		PrecompileHeaderSource = "%{prj.name}/src/common.cpp"

		files
		{
			"%{prj.name}/src/**.hpp",
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/src/**.asm"
		}

		includedirs
		{
			"%{IncludeDir.curlcpp}",
			"%{IncludeDir.json}",
			"%{prj.name}/src"
		}

		libdirs
		{
			"bin/lib"
		}

		links
		{
		
		}

		pchheader "%{PrecompileHeaderInclude}"
		pchsource "%{PrecompileHeaderSource}"

		forceincludes
		{
			"%{PrecompileHeaderInclude}"
		}

		DeclareMSVCOptions()
		DeclareDebugOptions()

		flags { "NoImportLib", "Maps" }

		filter "configurations:Debug"
			flags { "LinkTimeOptimization", "MultiProcessorCompile" }
			editandcontinue "Off"
			defines { "INJECTOR_DEBUG" }

		filter "configurations:Release"
			flags { "LinkTimeOptimization", "NoManifest", "MultiProcessorCompile" }
			defines { "INJECTOR_RELEASE" }
			optimize "speed"
