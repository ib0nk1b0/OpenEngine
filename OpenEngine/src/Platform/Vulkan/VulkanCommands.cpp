#include "oepch.h"
#include "VulkanCommands.h"

namespace OpenEngine {

	vk::CommandPool VulkanCommands::MakeCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
	{
		QueueFamilyIndices queueFamilyIndices = VulkanContext::FindQueueFamilies(physicalDevice, surface);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GetGraphicsFamily();

		try {
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to create CommandPool");
			return nullptr;
		}
	}

	vk::CommandBuffer VulkanCommands::MakeCommandBuffer(CommandBufferInputChunk inputChunk)
	{
		OE_CORE_INFO("Allocating command buffers...");
		
		vk::CommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.commandPool = inputChunk.CommandPool;
		allocateInfo.level = vk::CommandBufferLevel::ePrimary;
		allocateInfo.commandBufferCount = 1;

		for (auto& frame : inputChunk.Frames)
		{
			try {
				frame.CommandBuffer = inputChunk.Device.allocateCommandBuffers(allocateInfo)[0];
			}
			catch (vk::SystemError error)
			{
				OE_CORE_ASSERT(false, "Failed to allocate command buffer for frame");
			}
		}

		try {
			vk::CommandBuffer commandBuffer = inputChunk.Device.allocateCommandBuffers(allocateInfo)[0];

			return commandBuffer;
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to allocate main command buffer");
			return nullptr;
		}
	}

}