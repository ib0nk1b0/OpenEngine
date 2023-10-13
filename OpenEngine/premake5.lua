project "OpenEngine"
  kind "StaticLib"  
  language "C++"
  cppdialect "C++17"
  staticruntime "on"

  targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

  pchheader "oepch.h"
  pchsource "src/oepch.cpp"

  files
  {
    "src/**.h",
    "src/**.cpp",
    "vendor/stb_image/**.h",
    "vendor/stb_image/**.cpp",
    "vendor/glm/glm/**.hpp",
    "vendor/glm/glm/**.inl",
    "vendor/ImGuizmo/ImGuizmo.h",
    "vendor/ImGuizmo/ImGuizmo.cpp"
  }

  defines
  {
    "_CRT_SECURE_NO_WARNINGS"
  }

  includedirs
  {
    "%{wks.location}/OpenEngine/src",
    "%{wks.location}/OpenEngine/vendor/spdlog/include",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.Glad}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb_image}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.ImGuizmo}",
    "%{IncludeDir.json}",
    "%{IncludeDir.Assimp}"
    --"%{IncludeDir.VulkanSDK}"
  }

  links
  {
    "GLFW",
    "Glad",
    "ImGui",
    --"%{Library.Vulkan}",
    "opengl32.lib"
  }

  filter "files:vendor/ImGuizmo/**.cpp"
  flags { "NoPCH" }

  filter "system:windows"
    systemversion "latest"

    defines
    {
    	"OE_PLATFORM_WINDOWS",
        "SOME_RANDOM_DEFINE_AGAIN",
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