#include "oepch.h"
#include "Application.h"

#include "OpenEngine/Renderer/Renderer.h"

#include "Input.h"

//#include "Platform/Vulkan/VulkanCommands.h"
//#include "Platform/Vulkan/VulkanContext.h"
//#include "Platform/Vulkan/VulkanFramebuffer.h"

//#include <glfw/glfw3.h>
#include <filesystem>
#include <Platform/Windows/WindowsWindow.h>

struct AllocationMetrics
{
	uint32_t TotalAllocated = 0;
	uint32_t TotalFreed = 0;

	uint32_t CurrentUsage() { return TotalAllocated - TotalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

void* operator new(size_t size)
{
	s_AllocationMetrics.TotalAllocated += size;
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationMetrics.TotalFreed += size;
	free(memory);
}

namespace OpenEngine {

	// TODO: MOVE THESE SOMEWHERE ELSE
	// VULKAN FUNCTIONS THAT I DON'T UNDERSTAND
	//----------------------------------------------------------------
	/*vk::Semaphore MakeSemaphore(vk::Device device)
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
	}*/
	//----------------------------------------------------------------

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		OE_PROFILE_FUNCTION();

		OE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		//m_Window->SetVSync(false);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		Renderer::Init();
#if 0
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
#endif
	}

	Application::~Application()
	{
		OE_PROFILE_FUNCTION();
	}

	void Application::PushLayer(Layer* layer)
	{
		OE_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		OE_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		OE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		OE_PROFILE_FUNCTION();

		while (m_Running)
		{
			OE_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimised)
			{
				{
					OE_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}
			if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			{
				m_ImGuiLayer->Begin();
				{
					OE_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}
			else if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			{
				RenderCommand::Render();
			}
			m_Window->OnUpdate();

			auto memAllocated = s_AllocationMetrics.TotalAllocated;

			//std::cout << s_AllocationMetrics.CurrentUsage() << std::endl;
			CalculateFrameRate();
		}
	}

	void Application::CalculateFrameRate()
	{
		m_CurrentFrameTime = glfwGetTime();
		double delta = m_CurrentFrameTime - m_LastTime;

		if (delta >= 1)
		{
			int framerate{ std::max(1, int(m_NumberOfFrames / delta)) };
			std::stringstream title;
			title << m_Specification.Name <<" running at " << framerate << " fps.";
			glfwSetWindowTitle((GLFWwindow*)m_Window->GetNativeWindow(), title.str().c_str());
			m_LastTime = m_CurrentFrameTime;
			m_NumberOfFrames = -1;
			m_FrameTime = float(1000.0 / framerate);
		}

		++m_NumberOfFrames;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		OE_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimised = true;
			return false;
		}

		m_Minimised = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	/*void Application::Render()
	{
		m_DeviceSpec.Device.waitForFences(1, &m_InFilghtFence, VK_TRUE, UINT64_MAX);
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

	void Application::InitVulkanInstance()
	{
		m_VulkanInstance = VulkanContext::MakeInstance(m_Specification.Name.c_str());
		m_VulkanDLD = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);

		m_VulkanDebugMessenger = VulkanContext::MakeDebugMessenger(m_VulkanInstance, m_VulkanDLD);

		VkSurfaceKHR cStyleSurface;
		if (glfwCreateWindowSurface(m_VulkanInstance, (GLFWwindow*)m_Window->GetNativeWindow(), nullptr, &cStyleSurface) != VK_SUCCESS)
		{
			OE_CORE_WARN("Failed to abstract the glfw surface for Vulkan!");
		}

		OE_CORE_INFO("Successfully abstracted the glfw surface for Vulkan!");

		m_Surface = cStyleSurface;
	}

	void Application::InitVulkanDevice()
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

	void Application::InitVulkanPipeline()
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

	void Application::FinializeSetup()
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

	void Application::RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
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

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRenderPass();

		commandBuffer.end();
	}*/

}