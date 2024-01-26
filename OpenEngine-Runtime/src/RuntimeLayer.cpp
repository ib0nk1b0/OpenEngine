#include "RuntimeLayer.h"

#include <OpenEngine/Asset/AssetManager.h>
#include <OpenEngine/Project/Project.h>
#include <OpenEngine/Scripting/ScriptEngine.h>

#include <imgui/imgui.h>

namespace OpenEngine {

	RuntimeLayer::RuntimeLayer()
	{
	}

	void RuntimeLayer::OnAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_Scene = CreateRef<Scene>();

		// OpenProject
		AssetHandle startScene = 0;
		std::filesystem::path filepath = "SandboxProject/Sandbox.oeproj";
		if (Project::Load(filepath))
		{
			ScriptEngine::ReloadAssembly();
			startScene = Project::GetActive()->GetConfig().StartScene;
			Project::GetActive()->GetAssetManager()->DeserializeAssetRegistry();
		}

		if (startScene)
		{
			Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(startScene);
			Ref<Asset> asset = Project::GetActive()->GetAssetManager()->GetAsset(startScene);
			Ref<Scene> newScene = CreateRef<Scene>();
			readOnlyScene->CopyTo(newScene);
			m_Scene = newScene;
			auto filepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetAssetManager()->GetFilePath(startScene);
			m_Scene->SetFilepath(filepath);
			m_Scene->OnRuntimeStart();
		}

	}

	void RuntimeLayer::OnDetach()
	{
		m_Scene->OnRuntimeStop();
	}

	void RuntimeLayer::OnUpdate(OpenEngine::Timestep ts)
	{
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearColorAttachment(1, -1);

		m_Scene->OnUpdateRuntime(ts);

		m_Framebuffer->UnBind();
	}

	void RuntimeLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float windowMinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = windowMinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		//TODO: Rethink this approach to fix the camera not having correct aspectRatio until window is resized
		m_Scene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uint64_t)textureID, ImVec2{ 1600, 900 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void RuntimeLayer::OnEvent(OpenEngine::Event& e)
	{
	}

}
