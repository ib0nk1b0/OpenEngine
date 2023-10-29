#include "oepch.h"
#include "Application.h"

#include "OpenEngine/Core/Random.h"
#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/Debug/Instrumentor.h"

#include "Input.h"

#include <imgui.h>
#include <imgui_internal.h>

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
 	s_AllocationMetrics.TotalAllocated += (uint32_t)size;
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationMetrics.TotalFreed += (uint32_t)size;
	free(memory);
}

namespace OpenEngine {
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		OE_PROFILE_FUNCTION();

		OE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetVSync(specification.VSyncEnabled);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		if (specification.CustomTitlebar)
			m_Window->SetCustomTitlebar();

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		Renderer::Init();
		Random::Init();

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
				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

				//ImGuiViewport* viewport = ImGui::GetMainViewport();
				//ImGui::SetNextWindowPos(viewport->Pos);
				//ImGui::SetNextWindowSize(viewport->Size);
				//ImGui::SetNextWindowViewport(viewport->ID);
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				//window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
				///*if (!m_Specification.CustomTitleBar && m_MenubarCallback)
				//	window_flags |= ImGuiWindowFlags_MenuBar;*/

				//const bool isMaximised = IsMaximised();

				//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximised ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

				//ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
				//ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
				//ImGui::PopStyleColor(); // MenuBarBg
				//ImGui::PopStyleVar(2);

				//ImGui::PopStyleVar(2);

				//{
				//	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
				//	// Draw window border if the window is not maximized
				//	/*if (!isMaximized)
				//		UI::RenderWindowOuterBorders(ImGui::GetCurrentWindow());*/

				//	ImGui::PopStyleColor(); // ImGuiCol_Border
				//}

				/*if (m_Specification.CustomTitlebar)
				{
					float titleBarHeight;
					UI_DrawTitlebar(titleBarHeight);
					ImGui::SetCursorPosY(titleBarHeight);
				}*/

				// Dockspace
				/*ImGuiIO& io = ImGui::GetIO();
				ImGuiStyle& style = ImGui::GetStyle();
				float minWinSizeX = style.WindowMinSize.x;
				style.WindowMinSize.x = 370.0f;
				ImGui::DockSpace(ImGui::GetID("MyDockspace"));
				style.WindowMinSize.x = minWinSizeX;*/

				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();

				//ImGui::End();
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

	void Application::UI_DrawTitlebar(float& titlebarOutHeight)
	{
		const float titlebarHeight = 58.0f;
		const bool isMaximized = IsMaximised();
		float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebarHeight };
		auto* bgDrawList = ImGui::GetBackgroundDrawList();
		auto* fgDrawList = ImGui::GetForegroundDrawList();
		//bgDrawList->AddRectFilled(titlebarMin, titlebarMax, ImU32(0xFF0000FF));
		// DEBUG TITLEBAR BOUNDS
		fgDrawList->AddRect(titlebarMin, titlebarMax, ImU32(0xFF0000FF));

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float moveOffsetX;
		static float moveOffsetY;
		const float w = ImGui::GetContentRegionAvail().x;
		const float buttonsAreaWidth = 94;

		// Title bar drag area
		// On Windows we hook into the GLFW win32 window internals
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset)); // Reset cursor pos
		// DEBUG DRAG BOUNDS
		fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttonsAreaWidth, ImGui::GetCursorScreenPos().y + titlebarHeight), ImU32(0xFFFF0000));
		ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));

		m_TitlebarHovered = ImGui::IsItemHovered();
		//
		//if (isMaximized)
		//{
		//	float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
		//	if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
		//		m_TitlebarHovered = true; // Account for the top-most pixels which don't register
		//}

		// Draw Menubar
		if (m_MenubarCallback)
		{
			ImGui::SuspendLayout();
			{
				ImGui::SetItemAllowOverlap();
				const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + windowPadding.x;
				ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebarVerticalOffset));
				UI_DrawMenubar();

				if (ImGui::IsItemHovered())
					m_TitlebarHovered = false;
			}

			ImGui::ResumeLayout();
		}

		{
			// Centered Window title
			ImVec2 currentCursorPos = ImGui::GetCursorPos();
			ImVec2 textSize = ImGui::CalcTextSize(m_Specification.Name.c_str());
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f));
			ImGui::Text("%s", m_Specification.Name.c_str()); // Draw title
			ImGui::SetCursorPos(currentCursorPos);
		}

		// Window buttons
		const ImU32 buttonColN = 0xFF0000FF;
		const ImU32 buttonColH = 0xFF00FF00;
		const ImU32 buttonColP = 0xFFFF0000;
		const float buttonWidth = 14.0f;
		const float buttonHeight = 14.0f;

		// Minimize Button

		ImGui::Spring();
		//UI::ShiftCursorPosX(8.0f);
		{
			const int iconWidth = 32;
			const int iconHeight = 32;
			const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
			if (ImGui::Button("-"/*, ImVec2(buttonWidth, buttonHeight)*/))
			{
				// TODO: move this stuff to a better place, like Window class
				/*if (m_WindowHandle)
				{
					Application::Get().QueueEvent([windowHandle = m_WindowHandle]() { glfwIconifyWindow(windowHandle); });
				}*/
			}

			//UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));
		}


		// Maximize Button
		ImGui::Spring(-1.0f, 17.0f);
		//UI::ShiftCursorPosX(8.0f);
		{
			const int iconWidth = 32;
			const int iconHeight = 32;

			const bool isMaximized = IsMaximised();

			if (ImGui::Button("+"/*, ImVec2(buttonWidth, buttonHeight)*/))
			{
				/*Application::Get().QueueEvent([isMaximized, windowHandle = m_WindowHandle]()
					{
						if (isMaximized)
							glfwRestoreWindow(windowHandle);
						else
							glfwMaximizeWindow(windowHandle);
					});*/
			}

			//UI::DrawButtonImage(isMaximized ? m_IconRestore : m_IconMaximize, buttonColN, buttonColH, buttonColP);
		}

		// Close Button
		ImGui::Spring(-1.0f, 15.0f);
		//UI::ShiftCursorPosX(8.0f);
		{
			const int iconWidth = 32;
			const int iconHeight = 32;
			if (ImGui::Button("x"/*, ImVec2(buttonWidth, buttonHeight)*/))
				Application::Get().Close();

			//UI::DrawButtonImage(m_IconClose, UI::Colors::Theme::text, UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.4f), buttonColP);
		}

		ImGui::Spring(-1.0f, 18.0f);

		ImGui::EndHorizontal();
		titlebarOutHeight = titlebarHeight;
	}

	void Application::UI_DrawMenubar()
	{

	}

	bool Application::IsMaximised()
	{
		return (bool)glfwGetWindowAttrib((GLFWwindow*)m_Window.get(), GLFW_MAXIMIZED);
	}
}