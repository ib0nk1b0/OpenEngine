workspace "OpenEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "OpenEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "OpenEngine/vendor/Glad/include"
IncludeDir["Glad"] = "OpenEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "OpenEngine/vendor/imgui"

include "OpenEngine/vendor/GLFW"
include "OpenEngine/vendor/Glad"
include "OpenEngine/vendor/imgui"

project "OpenEngine"
	location "OpenEngine"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS",
			"OE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "OE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		optimize "On"
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "OE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		optimize "On"