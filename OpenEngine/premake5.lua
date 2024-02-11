project "OpenEngine"
  kind "StaticLib"  
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

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
    "vendor/ImGuizmo/ImGuizmo.cpp",
    "vendor/NFD-Extended/src/nfd_win.cpp",
    "vendor/NFD-Extended/include/NFD-Extended/**.h",
    "vendor/NFD-Extended/include/NFD-Extended/**.hpp"
  }

  defines
  {
    "_CRT_SECURE_NO_WARNINGS"
  }

  includedirs
  {
    "%{wks.location}/OpenEngine/src",
    "%{wks.location}/OpenEngine/vendor/spdlog/include",
    "%{IncludeDir.Filewatch}",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.Glad}",
    "%{IncludeDir.ImGui}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.stb_image}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.ImGuizmo}",
    "%{IncludeDir.json}",
    "%{IncludeDir.Assimp}",
    "%{IncludeDir.mono}",
    "%{IncludeDir.Box2D}",
    "%{IncludeDir.msdf_atlas_gen}",
    "%{IncludeDir.msdfgen}",
    "%{IncludeDir.NFD_Extended}",
    --"%{IncludeDir.VulkanSDK}"
  }

  links
  {
    "Box2D",
    "GLFW",
    "Glad",
    "ImGui",
    "msdf-atlas-gen",
    "opengl32.lib",
    
    "%{Library.Assimp}",
    "%{Library.mono}",
    --"%{Library.Vulkan}",
  }

  filter "files:vendor/ImGuizmo/**.cpp"
  flags { "NoPCH" }

  filter "files:vendor/Assimp/**.cpp"
  flags { "NoPCH" }

  filter "files:vendor/NFD-Extended/src/**.cpp"
  flags { "NoPCH" }

  filter "system:windows"
    systemversion "latest"

    defines
    {
    	"OE_PLATFORM_WINDOWS",
    	"GLFW_INCLUDE_NONE"
    }

    links
    {
      "%{Library.WinSock}",
      "%{Library.WinMM}",
      "%{Library.WinVersion}",
      "%{Library.BCrypt}"
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