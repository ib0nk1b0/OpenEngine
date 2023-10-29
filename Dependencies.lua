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
IncludeDir["Box2D"] = "%{wks.location}/OpenEngine/vendor/Box2D/include"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

--LibraryDir = {}
--LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

--Library = {}
--Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"