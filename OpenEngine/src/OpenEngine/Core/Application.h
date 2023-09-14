#pragma once

#include "Core.h"

#include "OpenEngine/Core/Window.h"
#include "OpenEngine/Core/LayerStack.h"
#include "OpenEngine/Events/Event.h"
#include "OpenEngine/Events/ApplicationEvent.h"

#include "OpenEngine/Core/Timestep.h"

#include "OpenEngine/ImGui/ImGuiLayer.h"

namespace OpenEngine {

	struct ApplicationSpecification
	{
		std::string Name = "OpenEngine Application";
		std::string WorkingDirectory;
		bool VSyncEnabled = false;
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

		inline Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
		void PrintCurrentUsage();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::string m_Name;
		ApplicationSpecification m_Specification;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimised = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;

		bool m_MemTracking = true;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}


