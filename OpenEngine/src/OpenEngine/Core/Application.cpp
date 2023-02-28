#include "oepch.h"
#include "Application.h"

#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/ImGui/ImGuiInfoLayer.h"

#include "Input.h"

#include <glfw/glfw3.h>

namespace OpenEngine {
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		OE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window->SetVSync(false);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
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

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimised)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimised = true;
			return false;
		}

		m_Minimised = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}