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
  include "OpenEngine/vendor/Box2D"
  include "OpenEngine/vendor/msdf-atlas-gen"
group ""

group "Core"
  include "OpenEngine"
  include "OpenEngine-ScriptCore"
group ""

group "Tools"
include "OpenEngine-Editor"
group ""

group "Runtime"
include "OpenEngine-Runtime"
group ""