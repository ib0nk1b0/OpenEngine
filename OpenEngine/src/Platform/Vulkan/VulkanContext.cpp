#include "oepch.h"
#include "VulkanContext.h"

#include "OpenEngine/Core/Application.h"

#include <set>

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

	bool VulkanContext::Supported(std::vector<const char*> extensions, std::vector<const char*> layers, bool debug)
	{
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		if (debug)
		{
			OE_CORE_INFO("Checking for supported extensions...");
			OE_CORE_TRACE("Device can support the following extensions");
			for (auto supportedExtension : supportedExtensions)
				OE_CORE_TRACE("\t{0}", supportedExtension.extensionName);
		}
		bool found = false;
		for (const char* extension : extensions)
		{
			found = false;
			for (vk::ExtensionProperties supported : supportedExtensions)
			{
				if (strcmp(extension, supported.extensionName) == 0)
				{
					found = true;
					if (debug)
						OE_CORE_TRACE("Extension \"{0}\" is supported", extension);
				}
			}

			if (!found)
			{
				if (debug)
					OE_CORE_TRACE("Extension \"{0}\" is not supported", extension);
				return false;
			}
		}

		std::vector<vk::LayerProperties> supprotedLayers = vk::enumerateInstanceLayerProperties();

		if (debug)
		{
			OE_CORE_INFO("Checking for supported layers...");
			OE_CORE_TRACE("Device can support the following layers:");
			for (vk::LayerProperties supportedLayer : supprotedLayers)
				OE_CORE_TRACE("\t{0}", supportedLayer.layerName);
		}

		for (const char* layer : layers)
		{
			found = false;
			for (vk::LayerProperties supported : supprotedLayers)
			{
				if (strcmp(layer, supported.layerName) == 0)
				{
					found = true;
					if (debug)
						OE_CORE_TRACE("Layer \"{0}\" is supported", layer);
				}
			}

			if (!found)
			{
				if (debug)
					OE_CORE_TRACE("Layer \"{0}\" is not supported", layer);
				return false;
			}
		}

		return true;
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
			OE_CORE_WARN("\tMajor: {0}", VK_API_VERSION_MAJOR(version));
			OE_CORE_WARN("\tMinor: {0}", VK_API_VERSION_MINOR(version));
			OE_CORE_WARN("\tPatch: {0}", VK_API_VERSION_PATCH(version));
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
			extensions.push_back("VK_EXT_debug_utils");
			OE_CORE_TRACE("Extensions to be requested:");

			for (const char* extensionName : extensions)
				OE_CORE_TRACE("\t\"{0}\"", extensionName);
		}

		std::vector<const char*> layers;
		if (debug)
			layers.push_back("VK_LAYER_KHRONOS_validation");

		if (!Supported(extensions, layers, debug))
			return nullptr;

		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			static_cast<uint32_t>(layers.size()), layers.data(), // enable layers
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

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT              messageType,
		const VkDebugUtilsMessengerCallbackDataEXT*  callbackData,
		void*                                        userData
	)
	{
		std::cerr << "Validation Layer: " << callbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	vk::DebugUtilsMessengerEXT VulkanContext::MakeDebugMessenger(vk::Instance instance, vk::DispatchLoaderDynamic dld)
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			DebugCallback,
			nullptr
		);

		return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dld);
	}

	static void LogDeviceProperties(const vk::PhysicalDevice device)
	{
		vk::PhysicalDeviceProperties properties = device.getProperties();
		OE_CORE_TRACE("Device Name: {0}", properties.deviceName);

		switch (properties.deviceType)
		{
			case (vk::PhysicalDeviceType::eCpu): 
				OE_CORE_TRACE("CPU");
				break;
			case (vk::PhysicalDeviceType::eDiscreteGpu):
				OE_CORE_TRACE("Discrete GPU");
				break;
			case (vk::PhysicalDeviceType::eIntegratedGpu):
				OE_CORE_TRACE("Integrated GPU");
				break;
			case (vk::PhysicalDeviceType::eVirtualGpu):
				OE_CORE_TRACE("Virtual GPU");
				break;
			default:
				OE_CORE_TRACE("Other");
		}
	}

	static bool DeviceExtensionsSupported(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, bool debug)
	{
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

		if (debug)
			OE_CORE_TRACE("Device can support extensions:");

		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			if (debug)
				OE_CORE_TRACE("\t{0}", extension.extensionName);

			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	static bool IsSuitable(vk::PhysicalDevice device, bool debug)
	{
		if (debug)
			OE_CORE_INFO("Checking if the device is suitable");

		const std::vector<const char*> requestedExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		if (debug)
		{
			OE_CORE_TRACE("We are requesting device extensions:");

			for (const char* extension : requestedExtensions)
				OE_CORE_TRACE("\t{0}", extension);
		}

		if (bool supported = DeviceExtensionsSupported(device, requestedExtensions, debug))
		{
			if (debug)
				OE_CORE_INFO("Device can support the requested extensions.");
		}
		else
		{
			if (debug)
				OE_CORE_WARN("Device can't support the requested extensions.");

			return false;
		}

		return true;
	}

	vk::PhysicalDevice VulkanContext::ChoosePhysicalDevice(vk::Instance instance, bool debug)
	{
		if (debug)
			OE_CORE_INFO("Choosing Physical Device...");

		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

		if (debug)
			OE_CORE_TRACE("There are {0} physical device(s) available on this system", availableDevices.size());

		for (vk::PhysicalDevice device : availableDevices)
		{
			if (debug)
				LogDeviceProperties(device);
			
			if (IsSuitable(device, debug))
				return device;
		}

		return nullptr;
	}

	void VulkanContext::SwapBuffers()
	{
	}

}