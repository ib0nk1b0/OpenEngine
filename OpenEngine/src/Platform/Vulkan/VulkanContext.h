#pragma once

#include "OpenEngine/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace OpenEngine {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext();

		virtual void Init();

		static vk::Instance MakeInstance(bool debug, const char* applicationName);
		static vk::DebugUtilsMessengerEXT MakeDebugMessenger(vk::Instance instance, vk::DispatchLoaderDynamic dld);
		static vk::PhysicalDevice ChoosePhysicalDevice(vk::Instance instance, bool debug);
		
		virtual void SwapBuffers();
	private:
		static bool Supported(std::vector<const char*> extensions, std::vector<const char*> layers, bool debug);
	private:
		GLFWwindow* m_WindowHandle;
	};

}