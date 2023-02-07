workspace "OpenEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.sysyem}-{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "OpenEngine/vendor/GLFW/include"

include "OpenEngine/vendor/GLFW"

project "OpenEngine"
	location "OpenEngine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "oepch.h"
	pchsource "OpenEngine/src/oepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS",
			"OE_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "OE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"OpenEngine/vendor/spdlog/include",
		"OpenEngine/src"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS"
		}

		links
		{
			"OpenEngine"
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "OE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"