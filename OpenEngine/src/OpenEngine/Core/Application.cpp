#include "oepch.h"
#include "Application.h"

#include "OpenEngine/Core/Random.h"
#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/Debug/Instrumentor.h"

#include "Input.h"

#include <glfw/glfw3.h>
#include <filesystem>

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
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		OE_PROFILE_FUNCTION();

		Random::Init();

		OE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetVSync(specification.VSyncEnabled);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
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
			//OE_PERF_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimised)
			{
				OE_PROFILE_SCOPE("LayerStack OnUpdate");
				OE_PERF_SCOPE("LayerStack::OnUpdates");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			{
				OE_PROFILE_SCOPE("LayerStack OnImGuiRender");
				OE_PERF_SCOPE("LayerStack::OnImGuiRender");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();

			//auto memAllocated = s_AllocationMetrics.TotalAllocated;
			//std::cout << s_AllocationMetrics.CurrentUsage() << std::endl;
		}
	}

	void OpenEngine::Application::PrintCurrentUsage()
	{
		if (!m_MemTracking) return;

		std::cout << s_AllocationMetrics.CurrentUsage() << " bytes\n";
	}

	void Application::AddTiming(const Timing& timing)
	{
		m_Timings.push_back(timing);
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
}