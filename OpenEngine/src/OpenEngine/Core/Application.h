#pragma once

#include "Core.h"

#include "OpenEngine/Core/Window.h"
#include "OpenEngine/Core/LayerStack.h"
#include "OpenEngine/Events/Event.h"
#include "OpenEngine/Events/ApplicationEvent.h"

#include "OpenEngine/Core/Timestep.h"

#include "OpenEngine/ImGui/ImGuiLayer.h"

#include "OpenEngine/Renderer/GraphicsContext.h"
//#include "Platform/Vulkan/VulkanGraphicsPipeline.h"

//#include <glfw/glfw3.h>
//#include <vulkan/vulkan.hpp>

namespace OpenEngine {
	
	struct SwapChainFrame;

	struct ApplicationSpecification
	{
		std::string Name = "OpenEngine Application";
		std::string WorkingDirectory;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }

		//void Render();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void CalculateFrameRate();
	private:
		//VULKAN STUFF
		/*void InitVulkanInstance();
		void InitVulkanDevice();
		void InitVulkanPipeline();

		void FinializeSetup();

		void RecordDrawCalls(vk::CommandBuffer commandBuffer, uint32_t imageIndex);*/
	private:
		std::string m_Name;
		ApplicationSpecification m_Specification;
		std::unique_ptr<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimised = false;
		LayerStack m_LayerStack;

		double m_CurrentFrameTime = 0.0f;
		double m_LastFrameTime = 0.0f;
		double m_LastTime = 0.0f;
		float m_FrameTime = 0.0f;
		int m_NumberOfFrames = 0;
	private:
		// VULKAN STUFF
		/*struct VulkanDeviceSpecification
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
		vk::Semaphore m_ImageAvailable, m_RenderFinished;*/
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}


