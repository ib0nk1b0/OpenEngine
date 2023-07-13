include "Dependencies.lua"

workspace "OpenEngine"
  architecture "x86_64"
  startproject "OpenEngine-Editor"

  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
  include "OpenEngine/vendor/GLFW"
  include "OpenEngine/vendor/Glad"
  include "OpenEngine/vendor/imgui"
group ""

group "Core"
  include "OpenEngine"
group ""

group "Tools"
include "OpenEngine-Editor"
group ""

group "Projects"
include "Sandbox"
group ""