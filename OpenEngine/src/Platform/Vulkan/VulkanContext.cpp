#include "oepch.h"
#include "VulkanContext.h"

#include "OpenEngine/Core/Application.h"

#include <GLFW/glfw3.h>

//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>

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

	vk::Instance VulkanContext::MakeInstance(bool debug, const char* applicationName)
	{
		if (debug)
			OE_CORE_WARN("Making a Vulkan instance...");

		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);
		
		if (debug)
		{
			OE_CORE_WARN("System can support vulkan variant: {0}", VK_API_VERSION_VARIANT(version));
			OE_CORE_WARN("    Major: {0}", VK_API_VERSION_MAJOR(version));
			OE_CORE_WARN("    Minor: {0}", VK_API_VERSION_MINOR(version));
			OE_CORE_WARN("    Patch: {0}", VK_API_VERSION_PATCH(version));
		}

		// select which version to use:
		// can either use the highest version and set path to 0
		version &= ~(0xFFFU);

		// Or set a specific version to ensure compatability
		// VK_MAKE_API_VERSION(variant, major, minor, patch)
		version = VK_MAKE_API_VERSION(0, 1, 0, 0);

		vk::ApplicationInfo appInfo = vk::ApplicationInfo(
			applicationName,
			version,
			"OpenEngine",
			version,
			version
		);

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (debug)
		{
			OE_CORE_TRACE("Extensions to be requested:");

			for (const char* extensionName : extensions)
				OE_CORE_TRACE("    \"{0}\"", extensionName);
		}

		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			0, nullptr, // enable layers
			static_cast<uint32_t>(extensions.size()), extensions.data()
		);

		try
		{
			return vk::createInstance(createInfo);
		}
		catch (vk::SystemError error)
		{
			if (debug)
				OE_CORE_ASSERT(false, "Failed to create instance!");

			return nullptr;
		}
	}

	void VulkanContext::SwapBuffers()
	{
	}

}