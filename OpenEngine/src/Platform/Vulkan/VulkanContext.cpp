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

	bool VulkanContext::Supported(std::vector<const char*> extensions, std::vector<const char*> layers)
	{
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

		if (OE_DEBUG)
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
					if (OE_DEBUG)
						OE_CORE_TRACE("Extension \"{0}\" is supported", extension);
				}
			}

			if (!found)
			{
				if (OE_DEBUG)
					OE_CORE_TRACE("Extension \"{0}\" is not supported", extension);
				return false;
			}
		}

		std::vector<vk::LayerProperties> supprotedLayers = vk::enumerateInstanceLayerProperties();

		if (OE_DEBUG)
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
					if (OE_DEBUG)
						OE_CORE_TRACE("Layer \"{0}\" is supported", layer);
				}
			}

			if (!found)
			{
				if (OE_DEBUG)
					OE_CORE_TRACE("Layer \"{0}\" is not supported", layer);
				return false;
			}
		}

		return true;
	}

	vk::Instance VulkanContext::MakeInstance(const char* applicationName)
	{
		if (OE_DEBUG)
			OE_CORE_WARN("Making a Vulkan instance...");

		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);
		
		if (OE_DEBUG)
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

		if (OE_DEBUG)
		{
			extensions.push_back("VK_EXT_debug_utils");
			OE_CORE_TRACE("Extensions to be requested:");

			for (const char* extensionName : extensions)
				OE_CORE_TRACE("\t\"{0}\"", extensionName);
		}

		std::vector<const char*> layers;
		if (OE_DEBUG)
			layers.push_back("VK_LAYER_KHRONOS_validation");

		if (!Supported(extensions, layers))
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
			if (OE_DEBUG)
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

	static bool DeviceExtensionsSupported(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions)
	{
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

		if (OE_DEBUG)
			OE_CORE_TRACE("Device can support extensions:");

		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			if (OE_DEBUG)
				OE_CORE_TRACE("\t{0}", extension.extensionName);

			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	static bool IsSuitable(vk::PhysicalDevice device)
	{
		if (OE_DEBUG)
			OE_CORE_INFO("Checking if the device is suitable");

		const std::vector<const char*> requestedExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		if (OE_DEBUG)
		{
			OE_CORE_TRACE("We are requesting device extensions:");

			for (const char* extension : requestedExtensions)
				OE_CORE_TRACE("\t{0}", extension);
		}

		if (bool supported = DeviceExtensionsSupported(device, requestedExtensions))
		{
			if (OE_DEBUG)
				OE_CORE_INFO("Device can support the requested extensions.");
		}
		else
		{
			if (OE_DEBUG)
				OE_CORE_WARN("Device can't support the requested extensions.");

			return false;
		}

		return true;
	}

	vk::PhysicalDevice VulkanContext::ChoosePhysicalDevice(vk::Instance instance)
	{
		if (OE_DEBUG)
			OE_CORE_INFO("Choosing Physical Device...");

		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

		if (OE_DEBUG)
			OE_CORE_TRACE("There are {0} physical device(s) available on this system", availableDevices.size());

		for (vk::PhysicalDevice device : availableDevices)
		{
			if (OE_DEBUG)
				LogDeviceProperties(device);
			
			if (IsSuitable(device))
				return device;
		}

		return nullptr;
	}

	vk::Device VulkanContext::CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(indices.GraphicsFamily.value());
		if (indices.GraphicsFamily.value() != indices.PresentFamily.value())
			uniqueIndices.push_back(indices.PresentFamily.value());

		float queuePriority = 1.0f;

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
		for (uint32_t queueFamilyIndex : uniqueIndices)
			queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority));

		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

		std::vector<const char*> enabledLayers;
		if (OE_DEBUG)
			enabledLayers.push_back("VK_LYAER_KHRONOS_validation");

		vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfo.size(), queueCreateInfo.data(),
			enabledLayers.size(), enabledLayers.data(),
			deviceExtensions.size(), deviceExtensions.data()
		);

		try
		{
			vk::Device device = physicalDevice.createDevice(deviceCreateInfo);
			if (OE_DEBUG)
				OE_CORE_INFO("GPU has been successfully abstracted");
			return device;
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ERROR("Device creation failed");
			return nullptr;
		}
	}

	void VulkanContext::SwapBuffers()
	{
	}

	std::array<vk::Queue, 2> VulkanContext::GetQueues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface)
	{
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

		return { {
				device.getQueue(indices.GraphicsFamily.value(), 0),
				device.getQueue(indices.PresentFamily.value(), 0)
		} };
	}

	QueueFamilyIndices VulkanContext::FindQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

		if (OE_DEBUG)
			OE_CORE_INFO("System can support {0} queue families", queueFamilies.size());

		int i = 0;
		for (const vk::QueueFamilyProperties& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.GraphicsFamily = i;

				if (OE_DEBUG)
					OE_CORE_INFO("Queue family {0} is suitable for graphics.", i);
			}

			if (physicalDevice.getSurfaceSupportKHR(i, surface))
			{
				indices.PresentFamily = i;

				if (OE_DEBUG)
					OE_CORE_INFO("Queue family {0} is suitable for presenting.", i);
			}

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails VulkanContext::QuerySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
	{
		SwapChainSupportDetails support;

		support.Capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		if (OE_DEBUG)
		{
			OE_CORE_TRACE("Swapchain can support the following surface capabilites");
			OE_CORE_TRACE("-------------------------------------------------------");
			OE_CORE_TRACE("\t Image count:");
			OE_CORE_TRACE("\t\tMinimum image count: {0}", support.Capabilities.minImageCount);
			OE_CORE_TRACE("\t\tMaximum image count: {0}", support.Capabilities.maxImageCount);
			OE_CORE_TRACE("\t Current extent:");
			OE_CORE_TRACE("\t\tWidth: {0}", support.Capabilities.currentExtent.width);
			OE_CORE_TRACE("\t\tHeight: {0}", support.Capabilities.currentExtent.height);
			OE_CORE_TRACE("\t Minimum extent:");
			OE_CORE_TRACE("\t\tWidth: {0}", support.Capabilities.minImageExtent.width);
			OE_CORE_TRACE("\t\tHeight: {0}", support.Capabilities.minImageExtent.height);
			OE_CORE_TRACE("\t Maximum extent:");
			OE_CORE_TRACE("\t\tWidth: {0}", support.Capabilities.maxImageExtent.width);
			OE_CORE_TRACE("\t\tHeight: {0}", support.Capabilities.maxImageExtent.height);
			OE_CORE_TRACE("\t Maximum image array layers: {0}", support.Capabilities.maxImageArrayLayers);
			OE_CORE_TRACE("-------------------------------------------------------");
		}

		support.Formats = physicalDevice.getSurfaceFormatsKHR(surface);

		support.PresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		return support;
	}

	vk::SurfaceFormatKHR VulkanContext::ChooseSwapchainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats)
	{
		for (vk::SurfaceFormatKHR format : formats)
		{
			if (format.format == vk::Format::eB8G8R8G8422Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return format;
		}

		return formats[0];
	}

	vk::PresentModeKHR VulkanContext::ChooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes)
	{
		for (vk::PresentModeKHR presentMode : presentModes)
		{
			if (presentMode == vk::PresentModeKHR::eMailbox)
				return presentMode;
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanContext::ChooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;
		else
		{
			vk::Extent2D extent = { width, height };

			extent.width = std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, width));
			extent.height = std::min(capabilities.maxImageExtent.height, std::max(capabilities.minImageExtent.height, height));

			return extent;
		}
	}

	SwapChainBundle VulkanContext::CreateSwapChain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height)
	{
		SwapChainSupportDetails support = QuerySwapChainSupport(physicalDevice, surface);

		vk::SurfaceFormatKHR format = ChooseSwapchainSurfaceFormat(support.Formats);

		vk::PresentModeKHR presentMode = ChooseSwapchainPresentMode(support.PresentModes);

		vk::Extent2D extent = ChooseSwapchainExtent(width, height, support.Capabilities);

		uint32_t imageCount = std::min(support.Capabilities.maxImageCount, support.Capabilities.minImageCount + 1);

		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		if (indices.GraphicsFamily.value() != indices.PresentFamily.value())
		{
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;

		createInfo.preTransform = support.Capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};

		try
		{
			bundle.Swapchain = logicalDevice.createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to create the swapchain!");
		}

		std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.Swapchain);
		bundle.Frames.resize(images.size());
		for (size_t i = 0; i < images.size(); i++)
		{
			vk::ImageViewCreateInfo createInfo = {};
			createInfo.image = images[i];
			createInfo.viewType = vk::ImageViewType::e2D;
			createInfo.components.r = vk::ComponentSwizzle::eIdentity;
			createInfo.components.g = vk::ComponentSwizzle::eIdentity;
			createInfo.components.b = vk::ComponentSwizzle::eIdentity;
			createInfo.components.a = vk::ComponentSwizzle::eIdentity;
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.format = format.format;

			bundle.Frames[i].Image = images[i];
			bundle.Frames[i].ImageView = logicalDevice.createImageView(createInfo);
		}

		bundle.Format = format.format;
		bundle.Extent = extent;

		return bundle;
	}

}