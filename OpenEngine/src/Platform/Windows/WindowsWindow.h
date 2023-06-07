#pragma once

#include "OpenEngine/Core/Window.h"
#include "OpenEngine/Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanGraphicsPipeline.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace OpenEngine {
	struct SwapChainFrame;
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }

		//void Render();
	private:
		virtual void Init(const WindowProps& props);

		/*void InitVulkanInstance();
		void InitVulkanDevice();
		void InitVulkanPipeline();

		void FinializeSetup();

		void RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex);*/

		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

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

		//VulkanGraphicsPipeline m_VulkanPipeline;

		VulkanDeviceSpecification m_DeviceSpec;
		VulkanSwapchainSpecification m_SwapchainSpec;
		VulkanPipelineSpecification m_PipelineSpecification;

		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_MainCommandBuffer;

		vk::Fence m_InFilghtFence;
		vk::Semaphore m_ImageAvailable, m_RenderFinished;

		struct WindowData
		{
			std::string Title;
			unsigned int Width = 0, Height = 0;
			bool VSync = true;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}