#include "oepch.h"
#include "ImGuiLayer.h"

#include "OpenEngine/Core/Application.h"
#include "OpenEngine/ImGui/ImGuiFonts.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

//TEMP
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace OpenEngine {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		OE_PROFILE_FUNCTION();

		//Setup Dear ImGui Context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		UI::FontConfiguration OpenSans;
		OpenSans.Name = "OpenSans";
		OpenSans.Filepath = "assets/fonts/Open_Sans/static/OpenSans/OpenSans-Regular.ttf";
		OpenSans.Size = 16.0f;
		UI::Fonts::Add(OpenSans, true);

		UI::FontConfiguration Bold;
		Bold.Name = "Bold";
		Bold.Filepath = "assets/fonts/Open_Sans/static/OpenSans/OpenSans-Bold.ttf";
		Bold.Size = 16.0f;
		UI::Fonts::Add(Bold);

		UI::FontConfiguration BoldLarge;
		BoldLarge.Name = "BoldLarge";
		BoldLarge.Filepath = "assets/fonts/Open_Sans/static/OpenSans/OpenSans-Bold.ttf";
		BoldLarge.Size = 24.0f;
		UI::Fonts::Add(BoldLarge);

		/*io.Fonts->AddFontFromFileTTF("assets/fonts/Open_Sans/static/OpenSans/OpenSans-Bold.ttf", 16.0f);
		io.Fonts->AddFontFromFileTTF("assets/fonts/Open_Sans/static/OpenSans/OpenSans-Bold.ttf", 24.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Open_Sans/static/OpenSans/OpenSans-Regular.ttf", 16.0f);*/

		// Setup styles
		//ImGui::StyleColorsClassic();
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		OE_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		OE_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		OE_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Window
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.094f, 0.094f, 0.105f, 1.0f };

		// Header
		colors[ImGuiCol_Header] = ImVec4{ 0.18f, 0.18f, 0.18f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Frames
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.04f, 0.04f, 0.06f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.14f, 0.14f, 0.16f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
	}
}