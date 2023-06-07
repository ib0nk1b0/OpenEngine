#include "oepch.h"
#include "WindowsWindow.h"

#include "OpenEngine/Events/ApplicationEvent.h"
#include "OpenEngine/Events/MouseEvent.h"
#include "OpenEngine/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

//#include "Platform/Vulkan/VulkanCommands.h"
#include "Platform/Vulkan/VulkanContext.h"
//#include "Platform/Vulkan/VulkanFramebuffer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_glfw.cpp>

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

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		OE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		OE_PROFILE_FUNCTION();

		Init(props);
		/*InitVulkanInstance();
		InitVulkanDevice();
		InitVulkanPipeline();
		FinializeSetup();*/
	}

	WindowsWindow::~WindowsWindow()
	{
		OE_PROFILE_FUNCTION();

		Shutdown();
	}

	/*void WindowsWindow::Render()
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

	}*/

	void WindowsWindow::Init(const WindowProps& props)
	{
		OE_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		OE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			}
			OE_CORE_ASSERT(success, "Could not initialize GLFW!");
			if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
				glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
		
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			glfwSetWindowUserPointer(m_Window, &m_Data);
			SetVSync(true);
			// Set GLFW callbacks

			glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

			glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

			glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

			glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

			glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

			glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
				MouseMovedEvent event((float)x, (float)y);
				data.EventCallback(event);
			});
		}
	}

	/*void WindowsWindow::InitVulkanInstance()
	{
		m_VulkanInstance = VulkanContext::MakeInstance(m_Data.Title.c_str());
		m_VulkanDLD = vk::DispatchLoaderDynamic(m_VulkanInstance, vkGetInstanceProcAddr);
		
		m_VulkanDebugMessenger = VulkanContext::MakeDebugMessenger(m_VulkanInstance, m_VulkanDLD);

		VkSurfaceKHR cStyleSurface;
		if (glfwCreateWindowSurface(m_VulkanInstance, m_Window, nullptr, &cStyleSurface) != VK_SUCCESS)
		{
			OE_CORE_WARN("Failed to abstract the glfw surface for Vulkan!");
		}

		OE_CORE_INFO("Successfully abstracted the glfw surface for Vulkan!");

		m_Surface = cStyleSurface;
	}

	void WindowsWindow::InitVulkanDevice()
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

	void WindowsWindow::InitVulkanPipeline()
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

	void WindowsWindow::FinializeSetup()
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

	void WindowsWindow::RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
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

	void WindowsWindow::Shutdown()
	{
		OE_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}

		for (auto frame : m_SwapchainSpec.Frames)
		{
			m_DeviceSpec.Device.destroyImageView(frame.ImageView);
			m_DeviceSpec.Device.destroyFramebuffer(frame.FrameBuffer);
		}

		m_DeviceSpec.Device.destroyCommandPool(m_CommandPool);
		m_DeviceSpec.Device.destroyFence(m_InFilghtFence);
		m_DeviceSpec.Device.destroyPipeline(m_PipelineSpecification.Pipeline);
		m_DeviceSpec.Device.destroyPipelineLayout(m_PipelineSpecification.Layout);
		m_DeviceSpec.Device.destroyRenderPass(m_PipelineSpecification.RenderPass);
		m_DeviceSpec.Device.destroySemaphore(m_ImageAvailable);
		m_DeviceSpec.Device.destroySemaphore(m_RenderFinished);
		m_DeviceSpec.Device.destroySwapchainKHR(m_SwapchainSpec.Swapchain);
		m_DeviceSpec.Device.destroy();

		m_VulkanInstance.destroySurfaceKHR(m_Surface);
		m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_VulkanDebugMessenger, nullptr, m_VulkanDLD);
		m_VulkanInstance.destroy();

	}

	void WindowsWindow::OnUpdate()
	{
		OE_PROFILE_FUNCTION();
		glfwPollEvents();

		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		OE_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}