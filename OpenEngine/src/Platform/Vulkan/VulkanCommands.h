#pragma once

#include "Platform/Vulkan/VulkanContext.h"
#include <vulkan/vulkan.hpp>

namespace OpenEngine {

	class VulkanCommands
	{
	public:
		struct CommandBufferInputChunk
		{
			vk::Device Device;
			vk::CommandPool CommandPool;
			std::vector<SwapChainFrame>& Frames;
		};
	public:
		static vk::CommandPool MakeCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
		static vk::CommandBuffer MakeCommandBuffer(CommandBufferInputChunk inputChunk);
	};

}