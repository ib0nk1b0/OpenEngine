#include "oepch.h"
#include "VulkanRendererAPI.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "Platform/Vulkan/VulkanCommands.h"

#include "OpenEngine/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace OpenEngine {

	struct PushConstantData
	{
		glm::vec2 Offset;
		alignas(16) glm::vec4 Color;
	};

	// TODO: MOVE THESE SOMEWHERE ELSE
	// VULKAN FUNCTIONS THAT I DON'T UNDERSTAND
	//----------------------------------------------------------------
	vk::Semaphore MakeSemaphore(vk::Device device)
	{
		vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(semaphoreCreateInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "");
			return nullptr;
		}
	}

	vk::Fence MakeFence(vk::Device device)
	{
		vk::FenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = vk::FenceCreateFlags();

		try {
			return device.createFence(fenceCreateInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "");
			return nullptr;
		}
	}
	//----------------------------------------------------------------

	void VulkanRendererAPI::Init()
	{
		InitVulkanInstance();
		InitVulkanDevice();
		InitVulkanPipeline();
		FinializeSetup();
	}

	void VulkanRendererAPI::InitVulkanInstance()
	{
		m_VulkanInstance = VulkanContext::MakeInstance("Default Title!");
		m_VulkanDLD = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);

		m_VulkanDebugMessenger = VulkanContext::MakeDebugMessenger(m_VulkanInstance, m_VulkanDLD);

		VkSurfaceKHR cStyleSurface;
		if (glfwCreateWindowSurface(m_VulkanInstance, (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), nullptr, &cStyleSurface) != VK_SUCCESS)
		{
			OE_CORE_WARN("Failed to abstract the glfw surface for Vulkan!");
		}

		OE_CORE_INFO("Successfully abstracted the glfw surface for Vulkan!");

		m_Surface = cStyleSurface;
	}

	void VulkanRendererAPI::InitVulkanDevice()
	{
		m_DeviceSpec.PhysicalDevice = VulkanContext::ChoosePhysicalDevice(m_VulkanInstance);
		m_DeviceSpec.Device = VulkanContext::CreateLogicalDevice(m_DeviceSpec.PhysicalDevice, m_Surface);
		std::array<vk::Queue, 2> queues = VulkanContext::GetQueues(m_DeviceSpec.PhysicalDevice, m_DeviceSpec.Device, m_Surface);
		m_DeviceSpec.GraphicsQueue = queues[0];
		m_DeviceSpec.PresentQueue = queues[1];
		SwapChainBundle bundle = VulkanContext::CreateSwapChain(m_DeviceSpec.Device, m_DeviceSpec.PhysicalDevice, m_Surface, 1600, 900);
		m_SwapchainSpec.Swapchain = bundle.Swapchain;
		m_SwapchainSpec.Frames = bundle.Frames;
		m_SwapchainSpec.Format = bundle.Format;
		m_SwapchainSpec.Extent = bundle.Extent;
	}

	void VulkanRendererAPI::InitVulkanPipeline()
	{
		GraphicsPipelineInBundle spec = {};
		spec.VertexPath = "assets/shaders/vertex.spv";
		spec.FragmentPath = "assets/shaders/fragment.spv";
		spec.Device = m_DeviceSpec.Device;
		spec.SwapchainExtent = m_SwapchainSpec.Extent;
		spec.SwapchainImageFormat = m_SwapchainSpec.Format;

		auto output = m_VulkanPipeline.CreateGraphicsPipeline(spec);
		m_PipelineSpecification.Layout = output.Layout;
		m_PipelineSpecification.RenderPass = output.RenderPass;
		m_PipelineSpecification.Pipeline = output.Pipeline;
	}

	void VulkanRendererAPI::FinializeSetup()
	{
		VulkanFramebufferInputSpecification framebufferInput;
		framebufferInput.Device = m_DeviceSpec.Device;
		framebufferInput.RenderPass = m_PipelineSpecification.RenderPass;
		framebufferInput.SwapchainExtent = m_SwapchainSpec.Extent;

		VulkanFramebuffer::MakeFramebuffer(framebufferInput, m_SwapchainSpec.Frames);
		m_CommandPool = VulkanCommands::MakeCommandPool(m_DeviceSpec.Device, m_DeviceSpec.PhysicalDevice, m_Surface);
		VulkanCommands::CommandBufferInputChunk inputChunk = { m_DeviceSpec.Device, m_CommandPool, m_SwapchainSpec.Frames };
		m_MainCommandBuffer = VulkanCommands::MakeCommandBuffer(inputChunk);

		m_InFilghtFence = MakeFence(m_DeviceSpec.Device);
		m_ImageAvailable = MakeSemaphore(m_DeviceSpec.Device);
		m_RenderFinished = MakeSemaphore(m_DeviceSpec.Device);
	}

	void VulkanRendererAPI::Render()
	{
		m_DeviceSpec.Device.waitForFences(1, &m_InFilghtFence, VK_TRUE, 1000000000);
		m_DeviceSpec.Device.resetFences(1, &m_InFilghtFence);

		uint32_t imageIndex{ m_DeviceSpec.Device.acquireNextImageKHR(m_SwapchainSpec.Swapchain, UINT64_MAX, m_ImageAvailable, nullptr).value };

		vk::CommandBuffer commandBuffer = m_SwapchainSpec.Frames[imageIndex].CommandBuffer;
		commandBuffer.reset();

		RecordDrawCalls(commandBuffer, imageIndex);

		vk::SubmitInfo submitInfo = {};
		vk::Semaphore waitSemaphores[] = { m_ImageAvailable };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vk::Semaphore signalSemaphores[] = { m_RenderFinished };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		m_DeviceSpec.GraphicsQueue.submit(submitInfo, m_InFilghtFence);

		vk::PresentInfoKHR presentInfo = {};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		vk::SwapchainKHR swapchains[] = { m_SwapchainSpec.Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		m_DeviceSpec.PresentQueue.presentKHR(presentInfo);

	}

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		//glm::vec2 TexCoord;
		//float TexIndex;
		//float Scale;

		//// Editor only
		//int EntityID;
	};

	void VulkanRendererAPI::RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
	{
		vk::CommandBufferBeginInfo beginInfo = {};
		try
		{
			commandBuffer.begin(beginInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to begin command buffer.");
		}

		vk::RenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.renderPass = m_PipelineSpecification.RenderPass;
		renderPassBeginInfo.framebuffer = m_SwapchainSpec.Frames[imageIndex].FrameBuffer;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = m_SwapchainSpec.Extent;

		vk::ClearValue clearColor = { std::array<float, 4>{ 1.0f, 0.0f, 1.0f, 1.0f } };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_PipelineSpecification.Pipeline);

		/*for (int i = 0; i < 4; i++)
		{
			PushConstantData push = {};
			push.Offset = { 0.0f, -0.4 + 0.25 * i };
			push.Color = { 0.0f, 0.0f, 0.2 + 0.2 * i, 1.0f };

			commandBuffer.pushConstants(m_PipelineSpecification.Layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof (PushConstantData), &push);
		}*/

		vk::VertexInputBindingDescription binding = {};
		binding.binding = 0;
		binding.stride = sizeof(QuadVertex);
		binding.inputRate = vk::VertexInputRate::eVertex;

		vk::VertexInputAttributeDescription attributes[] = {
			{
				0,
				binding.binding,
				vk::Format::eR32G32B32Sfloat,
				0
			},
			{
				1,
				binding.binding,
				vk::Format::eR32G32B32A32Sfloat,
				4 * sizeof(float)
			}
		};
		
		vk::PipelineVertexInputStateCreateInfo vertexInfo = {};
		vertexInfo.vertexBindingDescriptionCount = 1;
		vertexInfo.pVertexBindingDescriptions = &binding;
		vertexInfo.vertexAttributeDescriptionCount = 2;
		vertexInfo.pVertexAttributeDescriptions = attributes;
		
		

		//commandBuffer.setVertexInputEXT(binding, attributes);
		//commandBuffer.bindVertexBuffers(0, )

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRenderPass();

		commandBuffer.end();
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{

	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{

	}

	void VulkanRendererAPI::Clear()
	{

	}

	void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

	}

	void VulkanRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{

	}


}