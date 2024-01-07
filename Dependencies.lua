--VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/OpenEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/OpenEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/OpenEngine/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/OpenEngine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/OpenEngine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/OpenEngine/vendor/EnTT/include"
IncludeDir["json"] = "%{wks.location}/OpenEngine/vendor/Json/"
IncludeDir["ImGuizmo"] = "%{wks.location}/OpenEngine/vendor/ImGuizmo/"
IncludeDir["Assimp"] = "%{wks.location}/OpenEngine/vendor/Assimp/include"
IncludeDir["mono"] = "%{wks.location}/OpenEngine/vendor/mono/include"
IncludeDir["Box2D"] = "%{wks.location}/OpenEngine/vendor/Box2D/include"
IncludeDir["msdfgen"] = "%{wks.location}/OpenEngine/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/OpenEngine/vendor/msdf-atlas-gen/msdf-atlas-gen"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/OpenEngine/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["Assimp"] = "%{wks.location}/OpenEngine/vendor/Assimp/lib/x64"
--LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["Assimp"] = "%{LibraryDir.Assimp}/assimp-vc143-mt.lib"
--Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"