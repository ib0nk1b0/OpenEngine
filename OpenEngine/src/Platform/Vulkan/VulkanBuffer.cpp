#include "oepch.h"
#include "VulkanBuffer.h"
#include "OpenEngine/Renderer/RenderCommand.h"

#include <vulkan/vulkan.hpp>

namespace OpenEngine {

	uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice)
	{
		vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////
	// VertexBuffer //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
	{
		vk::Device& device = RenderCommand::s_RendererAPI->GetDevice();
		vk::PhysicalDevice& physicalDevice = RenderCommand::s_RendererAPI->GetPhysicalDevice();

		vk::BufferCreateInfo bufferInfo = {};
		bufferInfo.size = size;
		bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		vk::Buffer vertexBuffer = device.createBuffer(bufferInfo);
		vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(vertexBuffer);

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, physicalDevice);

		auto vertexBufferMemory = device.allocateMemory(allocInfo);

		device.bindBufferMemory(vertexBuffer, vertexBufferMemory, 0);

		void* data = device.mapMemory(vertexBufferMemory, 0, bufferInfo.size);
		memcpy(data, vertices, (size_t)bufferInfo.size);
		device.unmapMemory(vertexBufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}

	void VulkanVertexBuffer::Bind() const
	{
		
	}

	void VulkanVertexBuffer::UnBind() const
	{

	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
	}

	//////////////////////////////////////////////////////////////////////
	// IndexBuffer ///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
	}

	void VulkanIndexBuffer::Bind() const
	{
	}

	void VulkanIndexBuffer::UnBind() const
	{
	}

}