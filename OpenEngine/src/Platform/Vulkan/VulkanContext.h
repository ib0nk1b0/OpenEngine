#pragma once

#include "OpenEngine/Renderer/GraphicsContext.h"

#include <optional>

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace OpenEngine {

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete()
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}

		uint32_t GetGraphicsFamily()
		{
			return GraphicsFamily.has_value() ? GraphicsFamily.value() : 0;
		}

		uint32_t GetPresentFamily()
		{
			return PresentFamily.has_value() ? PresentFamily.value() : 0;
		}
	};

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR> PresentModes;
	};

	struct SwapChainFrame
	{
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer FrameBuffer;
		vk::CommandBuffer CommandBuffer;
	};

	struct SwapChainBundle
	{
		vk::SwapchainKHR Swapchain;
		std::vector<SwapChainFrame> Frames;
		vk::Format Format;
		vk::Extent2D Extent;
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext();

		virtual void Init();

		static vk::Instance MakeInstance(const char* applicationName);
		static vk::DebugUtilsMessengerEXT MakeDebugMessenger(vk::Instance instance, vk::DispatchLoaderDynamic dld);
		static vk::PhysicalDevice ChoosePhysicalDevice(vk::Instance instance);
		static vk::Device CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
		static SwapChainBundle CreateSwapChain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height);
		
		virtual void SwapBuffers();

		static std::array<vk::Queue, 2> GetQueues(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface);
		static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
		static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
		static vk::SurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats);
		static vk::PresentModeKHR ChooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes);
		static vk::Extent2D ChooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities);
	private:
		static bool Supported(std::vector<const char*> extensions, std::vector<const char*> layers);
	private:
		GLFWwindow* m_WindowHandle;
	};

}