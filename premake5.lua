workspace "OpenEngine"
  architecture "x64"
  startproject "OpenEngine-Editor"

  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "OpenEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "OpenEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "OpenEngine/vendor/imgui"
IncludeDir["glm"] = "OpenEngine/vendor/glm"
IncludeDir["stb_image"] = "OpenEngine/vendor/stb_image"
IncludeDir["entt"] = "OpenEngine/vendor/EnTT/include"
IncludeDir["json"] = "OpenEngine/vendor/Json/"
IncludeDir["ImGuizmo"] = "OpenEngine/vendor/ImGuizmo/"

group "Dependencies"
  include "OpenEngine/vendor/GLFW"
  include "OpenEngine/vendor/Glad"
  include "OpenEngine/vendor/imgui"
group ""

project "OpenEngine"
  location "OpenEngine"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

  pchheader "oepch.h"
  pchsource "OpenEngine/src/oepch.cpp"

  files
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/vendor/stb_image/**.h",
    "%{prj.name}/vendor/stb_image/**.cpp",
    "%{prj.name}/vendor/glm/glm/**.hpp",
    "%{prj.name}/vendor/glm/glm/**.inl",
    "%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
    "%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
  }

  defines
  {
    "_CRT_SECURE_NO_WARNINGS"
  }

  includedirs
  {
    "%{prj.name}/src",
    "%{prj.name}/vendor/spdlog/include",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.Glad}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb_image}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.ImGuizmo}",
    "%{IncludeDir.json}"
  }

  links
  {
    "GLFW",
    "Glad",
    "ImGui",
    "opengl32.lib"
  }

  filter "files:OpenEngine/vendor/ImGuizmo/**.cpp"
  flags { "NoPCH" }

  filter "system:windows"
    systemversion "latest"

    defines
    {
    	"OE_PLATFORM_WINDOWS",
    	"OE_BUILD_DLL",
    	"GLFW_INCLUDE_NONE"
    }

  filter "configurations:Debug"
    defines "OE_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "OE_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "OE_DIST"
    runtime "Release"
    optimize "on"

project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

  files
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp",
    "%{prj.name}/vendor/glm/glm/**.hpp",
    "%{prj.name}/vendor/glm/glm/**.inl"
  }

  includedirs
  {
    "OpenEngine/vendor/spdlog/include",
    "OpenEngine/src",
    "OpenEngine/vendor",
    "%{IncludeDir.glm}",
    "%{IncludeDir.entt}"
  }

  links
  {
    "OpenEngine"
  }

  filter "system:windows"
    systemversion "latest"

    defines
    {
      "OE_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines "OE_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "OE_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "OE_DIST"
    runtime "Release"
    optimize "on"

project "OpenEngine-Editor"
  location "OpenEngine-Editor"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

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
    "OpenEngine/src",
    "OpenEngine/vendor",
    "%{IncludeDir.glm}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.json}"
  }

  links
  {
    "OpenEngine"
  }

  filter "system:windows"
    systemversion "latest"

    defines
    {
      "OE_PLATFORM_WINDOWS"
    }

  filter "configurations:Debug"
    defines "OE_DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "OE_RELEASE"
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines "OE_DIST"
    runtime "Release"
    optimize "on"
