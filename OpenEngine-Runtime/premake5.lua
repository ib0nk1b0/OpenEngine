project "OpenEngine-Runtime"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  staticruntime "off"

  targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

  files
  {
    "src/**.h",
    "src/**.cpp"
  }

  includedirs
  {
    "%{wks.location}/OpenEngine/vendor/spdlog/include",
		"%{wks.location}/OpenEngine/src",
		"%{wks.location}/OpenEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
  }

  links
  {
    "OpenEngine"
  }

  postbuildcommands {
    "{COPY} %{Library.Assimp} %{cfg.targetdir}"
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

