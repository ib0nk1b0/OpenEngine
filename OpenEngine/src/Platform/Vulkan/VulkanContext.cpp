#include "oepch.h"
#include "VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace OpenEngine {

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
	}

	VulkanContext::~VulkanContext()
	{
	}

	void VulkanContext::Init()
	{
	}

	void VulkanContext::SwapBuffers()
	{
	}

}