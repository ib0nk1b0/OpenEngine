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
		PushLayer(new ImGuiInfoLayer());

		OE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window->SetVSync(false);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

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

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			float thisFramePerSecondCount = 1 / timestep.GetSeconds();
			float runningTotal = 0.0f;
			int sizeOfArray = sizeof(m_PreviousFrames) / sizeof(*m_PreviousFrames);
			
			for (int i = 0; i < sizeOfArray; i++)
			{
				runningTotal += m_PreviousFrames[i];
				if (i != 0)
					m_PreviousFrames[i - 1] = m_PreviousFrames[i];
			}

			m_PreviousFrames[sizeOfArray - 1] = thisFramePerSecondCount;

			m_FramesPerSecond = floor(runningTotal / sizeOfArray);

			OE_CORE_INFO("Fps: {0}", m_FramesPerSecond);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep, m_FramesPerSecond);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
}