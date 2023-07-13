#pragma once

#include "OpenEngine/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanGraphicsPipeline.h"
#include "Platform/Vulkan/VulkanBuffer.h"

#include <vulkan/vulkan.hpp>

namespace OpenEngine {

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		
		virtual void Render() override;
		virtual vk::Device GetDevice() override { return m_DeviceSpec.Device; }
		virtual vk::PhysicalDevice GetPhysicalDevice() override { return m_DeviceSpec.PhysicalDevice; }
	private:
		void InitVulkanInstance();
		void InitVulkanDevice();
		void InitVulkanPipeline();
		void FinializeSetup();
		void RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
	private:
		struct VulkanDeviceSpecification
		{
			vk::Device Device{ nullptr };
			vk::PhysicalDevice PhysicalDevice{ nullptr };
			vk::Queue GraphicsQueue{ nullptr };
			vk::Queue PresentQueue{ nullptr };
		};

		struct VulkanSwapchainSpecification
		{
			vk::SwapchainKHR Swapchain;
			std::vector<SwapChainFrame> Frames;
			vk::Format Format;
			vk::Extent2D Extent;
		};

		struct VulkanPipelineSpecification
		{
			vk::PipelineLayout Layout;
			vk::RenderPass RenderPass;
			vk::Pipeline Pipeline;
		};

		vk::Instance m_VulkanInstance{ nullptr };
		vk::DebugUtilsMessengerEXT m_VulkanDebugMessenger{ nullptr };
		vk::DispatchLoaderDynamic m_VulkanDLD;
		vk::SurfaceKHR m_Surface;

		VulkanGraphicsPipeline m_VulkanPipeline;

		VulkanDeviceSpecification m_DeviceSpec;
		VulkanSwapchainSpecification m_SwapchainSpec;
		VulkanPipelineSpecification m_PipelineSpecification;

		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_MainCommandBuffer;

		vk::Fence m_InFilghtFence;
		vk::Semaphore m_ImageAvailable, m_RenderFinished;
		friend class VulkanVertexBuffer;
	};

}