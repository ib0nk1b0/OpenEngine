project "Assimp"
  kind "StaticLib"
  language "C"
  staticruntime "on"

  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("bin-int/" ..outputdir .. "/%{prj.name}")

  files 
  {
    "include/assimp/**.h",
    "include/assimp/**.hpp",
    "include/assimp/**.c"
  }

  includedirs
  {
    "include"
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"
