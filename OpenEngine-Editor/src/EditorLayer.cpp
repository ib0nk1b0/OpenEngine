#include "EditorLayer.h"

#include "OpenEngine/Renderer/Renderer2D.h"
#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/Math/Math.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <EnTT/include/entt.hpp>

#include "FPSCameraController.h"
#include "FlappyBird/Player.h"
#include "FlappyBird/Camera.h"
#include "FlappyBird/Ground.h"

namespace OpenEngine {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		OE_PROFILE_FUNCTION();

		m_PlayIcon = Texture2D::Create("assets/icons/Play.png");
		m_StopIcon = Texture2D::Create("assets/icons/Stop.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_RuntimeScene = CreateRef<Scene>();

		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_MaterialPanel.SetContext(m_EditorScene);

		m_EditorCamera = EditorCamera(60.0f, 1.778f, 0.1f, 1000.0f);
	}

	void EditorLayer::OnDetach()
	{
		OE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		OE_PROFILE_FUNCTION();

		m_FrameTime = ts;
		m_EditorCamera.OnUpdate(ts);

		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearColorAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
				m_EditorScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			case SceneState::Play:
				m_RuntimeScene->OnUpdateRuntime(ts);
				break;
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_EditorScene.get());
		}

		m_Framebuffer->UnBind();
	}

	void EditorLayer::OnImGuiRender()
	{
		OE_PROFILE_FUNCTION();

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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 370.0f;

		UI_MenuBar();

		if (m_DisplayStats)
			UI_Stats();

		if (m_DisplayEditorCameraUI)
			UI_EditorCameraPanel();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();
		m_MaterialPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		//TODO: Rethink this approach to fix the camera not having correct aspectRatio until window is resized
		m_EditorScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
		m_EditorCamera.SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uint64_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1 && m_SceneState != SceneState::Play)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;

			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(OE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(OE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::ScenePlay()
	{
		if (m_EditorScene->GetFilepath() == "assets\\Scenes\\game.openengine")
		{
			if (!m_EditorScene->GetEntityByName("Player").HasComponent<NativeScriptComponent>())
				m_EditorScene->GetEntityByName("Player").AddComponent<NativeScriptComponent>().Bind<Player>();
			if (!m_EditorScene->GetEntityByName("Camera").HasComponent<NativeScriptComponent>())
				m_EditorScene->GetEntityByName("Camera").AddComponent<NativeScriptComponent>().Bind<FPSCameraController>();
		}

		m_SceneState = SceneState::Play;
		m_EditorScene->CopyTo(m_RuntimeScene);
		m_RuntimeScene->SetFilepath(m_EditorScene->GetFilepath());
		m_SceneHierarchyPanel.SetContext(m_RuntimeScene);
		m_RuntimeScene->OnRuntimeStart();
	}

	void EditorLayer::SceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_RuntimeScene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_EditorScene->OnEditorStart();
	}

	void EditorLayer::UI_MenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Project", "Ctrl+Shift+N"))
						NewProject();

					if (ImGui::MenuItem("Scene", "Ctrl+N"))
						NewScene();

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Open", "Ctrl+O"))
					OpenScene();

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Menus"))
			{
				if (ImGui::MenuItem("Scene Hierarchy", NULL, &m_DisplaySceneHierarchy))
					m_SceneHierarchyPanel.DisplaySceneHierarchy(m_DisplaySceneHierarchy);

				if (ImGui::MenuItem("Properties", NULL, &m_DisplayProperties))
					m_SceneHierarchyPanel.DisplayProperties(m_DisplayProperties);

				if (ImGui::MenuItem("Materials", NULL, &m_DisplayMaterials))
					m_MaterialPanel.Display(m_DisplayMaterials);

				ImGui::MenuItem("Renderer Stats", NULL, &m_DisplayStats);
				ImGui::MenuItem("Editor Camera UI", NULL, &m_DisplayEditorCameraUI);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::UI_Stats()
	{
		ImGui::Begin("Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

		ImGui::Separator();
		ImGui::Text("Frame Time: %f", m_FrameTime.GetMilliseconds());
		
		ImGui::Separator();
		float gridSize = m_EditorScene->GetGridSize();
		if (ImGui::OEDragFloat("Grid size", &gridSize, 1))
			m_EditorScene->SetGridSize(gridSize);

		ImGui::End();
	}

	void EditorLayer::UI_EditorCameraPanel()
	{
		ImGui::Begin("Editor Camera");

		float fov = m_EditorCamera.GetFOV();
		if (ImGui::OEDragFloat("FOV", &fov, 0.5f, 0.0f, 120.0f))
			m_EditorCamera.SetFOV(fov);

		float nearClip = m_EditorCamera.GetNearClip();
		if (ImGui::OEDragFloat("Near Clip", &nearClip))
			m_EditorCamera.SetNearClip(nearClip);

		float farClip = m_EditorCamera.GetFarClip();
		if (ImGui::OEDragFloat("Far Clip", &farClip))
			m_EditorCamera.SetFarClip(farClip);

		float pitch = m_EditorCamera.GetPitch();
		if (ImGui::OEDragFloat("Pitch", &pitch, 0.1f))
			m_EditorCamera.SetPitch(pitch);

		float yaw = m_EditorCamera.GetYaw();
		if (ImGui::OEDragFloat("Yaw", &yaw, 0.1f))
			m_EditorCamera.SetYaw(yaw);

		ImGui::Checkbox("Rotation enabled", &m_EditorCamera.m_Rotate);

		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		float size = ImGui::GetWindowHeight() - 4;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayIcon : m_StopIcon;
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f - size * 0.5f);
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				ScenePlay();
			else if (m_SceneState == SceneState::Play)
				SceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control && shift)
					NewProject();
				else if (control)
					NewScene();

				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();

				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control)
					SaveScene();

				break;
			}
			case Key::F5:
			{
				if (control)
				{
					if (m_SceneState == SceneState::Edit)
					{
						ScenePlay();

						break;
					}
					if (m_SceneState == SceneState::Play)
					{
						SceneStop();

						break;
					}
				}

				break;
			}
			case Key::Escape:
			{
				m_RuntimeScene->ToggleCursor();
			}
			case Key::G:
			{
				if (alt)
				{
					m_EditorScene->ToggleGrid();
				}
			}

			// Gizmo shortcuts
			if (m_SceneState == SceneState::Edit)
			{
				case Key::Q:
					m_GizmoType = -1;
					break;
				case Key::W:
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					break;
				case Key::E:
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					break;
				case Key::R:
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
					break;
			}
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft && m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);

		return false;
	}

	void EditorLayer::NewProject()
	{
		OE_WARN("Projects are not yet developed."); // TODO: <- do this
	}

	void EditorLayer::NewScene(const std::string& filepath)
	{
		std::vector<Material> materials = m_EditorScene->GetMaterials();
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->SetFilepath(filepath);
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorScene->SetMaterials(materials);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_MaterialPanel.SetContext(m_EditorScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("OpenEngine Scene (*.openengine)\0*.openengine\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& filepath)
	{
		if (FileDialogs::IsValidFile(filepath, ".openengine"))
		{
			NewScene(filepath.string());

			Serializer serializer(m_EditorScene);
			serializer.Deserialize(filepath.string());
		}
	}

	void EditorLayer::SaveScene()
	{
		const std::string filepath = m_EditorScene->GetFilepath();
		if (filepath == "UntitledScene.openengine")
			SaveSceneAs();
		else
		{
			Serializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("OpenEngine Scene (*.openengine)\0*.openengine\0");
		if (!filepath.empty())
		{
			Serializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
			m_EditorScene->SetFilepath(filepath);
		}
	}

}
