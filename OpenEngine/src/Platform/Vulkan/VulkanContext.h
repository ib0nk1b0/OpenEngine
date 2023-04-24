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
		virtual void SwapBuffers();
	private:
		GLFWwindow* m_WindowHandle;
	};

}