#include "EditorLayer.h"

#include "OpenEngine/Asset/AssetManager.h"
#include "OpenEngine/Asset/TextureImporter.h"
#include "OpenEngine/Math/Math.h"
#include "OpenEngine/Renderer/Renderer2D.h"
#include "OpenEngine/Renderer/Font.h"
#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Scripting/ScriptEngine.h"
#include "OpenEngine/Utils/PlatformUtils.h"

#include "OpenEngine/ImGui/ImGuiFonts.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <EnTT/include/entt.hpp>

namespace OpenEngine {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		OE_PROFILE_FUNCTION();

		m_PlayIcon = TextureImporter::LoadTexture2D("Resources/Icons/PlayButton.png");
		m_StopIcon = TextureImporter::LoadTexture2D("Resources/Icons/StopButton.png");

		SceneHierarchyPanel::Init();
		MaterialPanel::Init();

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_RuntimeScene = CreateRef<Scene>();

		SceneHierarchyPanel::SetContext(m_EditorScene);
		MaterialPanel::SetContext(m_EditorScene);

		OpenProject("SandboxProject/Sandbox.oeproj");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		/*
		* 
		* 1000 cubes 2-3ms frame time in release mode
		* most of the time spent in Scene Update, probably due to mesh->SetData()
		* *//*
		m_EditorScene->CreateEntity("Light").AddComponent<PointLightComponent>();
		m_EditorScene->GetEntityByName("Light").GetComponent<TransformComponent>().Translation = { -0.2f, 0.5f, 0.3f };

		for (float x = -5; x < 5; x++)
		{
			for (int y = 1; y < 11; y++)
			{
				for (int z = -5; z < 5; z++)
				{
					Entity cube = m_EditorScene->CreateEntity("Cube");
					cube.AddComponent<MeshComponent>();
					cube.GetComponent<MeshComponent>().Filepath = Utils::FormatFilepath("assets\\Models\\Cube.obj");
					cube.GetComponent<TransformComponent>().Translation = { x, y, z };
					cube.GetComponent<TransformComponent>().Scale = { 0.8f, 0.8f, 0.8f };
				}
			}
		}*/
	}

	void EditorLayer::OnDetach()
	{
		OE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		OE_PROFILE_FUNCTION();
		OE_PERF_SCOPE("EditorLayer::OnUpdate");

		m_FrameTime = ts;
		m_EditorCamera.OnUpdate(ts);

		Renderer2D::ResetStats();
		Renderer::ResetStats();

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
		m_MouseX = (int)mx;
		m_MouseY = (int)my;

		if (m_MouseX >= 0 && m_MouseY >= 0 && m_MouseX < (int)m_ViewportSize.x && m_MouseY < (int)m_ViewportSize.y && Input::IsMouseButtonPressed(Mouse::ButtonLeft) && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
		{
			int pixelData = m_Framebuffer->ReadPixel(1, m_MouseX, m_MouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_EditorScene.get());
			SceneHierarchyPanel::SetSelectedEntity(m_HoveredEntity);
		}

		RenderOverlay();

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
		ImGuiStyle& style = ImGui::GetStyle();
		float windowMinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = windowMinSizeX;

		/*ImGui::Begin("Popups");
		
		if (ImGui::Button("Popup"))
			ImGui::OpenPopup("popup1");

		bool open = true;
		if (ImGui::BeginPopupModal("popup1", &open))
		{
			ImGui::Text("This is a popup!");
			ImGui::EndPopup();
		}

		ImGui::End();*/

		UI_MenuBar();

		if (m_DisplayStats)
			UI_Stats();

		if (m_DisplayEditorCameraUI)
			UI_EditorCameraPanel();

		if (m_DisplayProjectPopup)
			UI_NewProject();

		MaterialPanel::OnImGuiRender();
		SceneHierarchyPanel::OnImGuiRender();
		AssetManagerPanel::OnImGuiRender();
		ContentBrowserPanel::OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

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
				AssetHandle handle = *(AssetHandle*)payload->Data;
				OpenScene(handle);
			}
			ImGui::EndDragDropTarget();
		}

		Entity selectedEntity = SceneHierarchyPanel::GetSelectedEntity();
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
		//UI_ColorScheme();

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
		m_SceneState = SceneState::Play;
		m_EditorScene->CopyTo(m_RuntimeScene);
		m_RuntimeScene->SetFilepath(m_EditorScene->GetFilepath());
		SceneHierarchyPanel::SetContext(m_RuntimeScene);
		m_RuntimeScene->OnRuntimeStart();
	}

	void EditorLayer::SceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_RuntimeScene->OnRuntimeStop();
		m_RuntimeScene = CreateRef<Scene>();
		SceneHierarchyPanel::SetContext(m_RuntimeScene);
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
				if (ImGui::MenuItem("Scene Hierarchy", NULL, SceneHierarchyPanel::DisplaySceneHirerarchy()))
					SceneHierarchyPanel::ToggleSceneHierarchy();

				if (ImGui::MenuItem("Properties", NULL, SceneHierarchyPanel::DisplayProperties()))
					SceneHierarchyPanel::ToggleProperties();

				if (ImGui::MenuItem("Materials", NULL, &m_DisplayMaterials))
					MaterialPanel::ToggleDisplay();

				ImGui::MenuItem("Renderer Stats", NULL, &m_DisplayStats);
				ImGui::MenuItem("Editor Camera UI", NULL, &m_DisplayEditorCameraUI);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scripts"))
			{
				if (ImGui::MenuItem("Reload Assembly", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

			ImGui::Begin("Settings");

			ImGui::Checkbox("Physics Colliders", &m_DisplayPhysicsColliders);

			ImGui::End();
		}
	}

	void EditorLayer::UI_Stats()
	{
		{
			ImGui::Begin("Renderer Stats");

			ImGui::Text("Renderer Stats:");
			ImGui::Text("Draw Calls: %d", Renderer::GetStatistics().DrawCalls);
			ImGui::Text("Meshes: %d", Renderer::GetStatistics().Meshes);

			ImGui::Separator();
			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

			ImGui::Separator();
			int gridSize = m_EditorScene->GetGridSize();
			if (UI::DragInt("Grid size", &gridSize))
				m_EditorScene->SetGridSize(gridSize);

			ImGui::End();
		}

		ImGui::Begin("Performance Stats");
		ImGui::Text("Frame Time: %f", m_FrameTime.GetMilliseconds());
		
		auto timings = Application::Get().GetApplicationTimings();

		for (Timing& timing : timings)
			ImGui::Text("%s: %f ms", timing.Name.c_str(), timing.Time);

		Application::Get().ClearTimings();
		ImGui::End();
	}

	void EditorLayer::UI_EditorCameraPanel()
	{
		ImGui::Begin("Editor Camera");

		float fov = m_EditorCamera.GetFOV();
		if (UI::DragFloat("FOV", &fov, 0.5f, 0.0f, 120.0f))
			m_EditorCamera.SetFOV(fov);

		float nearClip = m_EditorCamera.GetNearClip();
		if (UI::DragFloat("Near Clip", &nearClip))
			m_EditorCamera.SetNearClip(nearClip);

		float farClip = m_EditorCamera.GetFarClip();
		if (UI::DragFloat("Far Clip", &farClip))
			m_EditorCamera.SetFarClip(farClip);

		float pitch = m_EditorCamera.GetPitch();
		if (UI::DragFloat("Pitch", &pitch, 0.1f))
			m_EditorCamera.SetPitch(pitch);

		float yaw = m_EditorCamera.GetYaw();
		if (UI::DragFloat("Yaw", &yaw, 0.1f))
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

		{
			UI::ScopedFont bold(UI::Fonts::Get("Bold"));
			ImGui::Text("%s", m_EditorScene->GetFilepath().filename().string().c_str());
			ImGui::SameLine();
		}

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

	void EditorLayer::UI_ColorScheme()
	{
		auto& colors = ImGui::GetStyle().Colors;

		glm::vec3 WindowBg = { colors[ImGuiCol_WindowBg].x, colors[ImGuiCol_WindowBg].y, colors[ImGuiCol_WindowBg].z };
		glm::vec3 Header = { colors[ImGuiCol_Header].x, colors[ImGuiCol_Header].y, colors[ImGuiCol_Header].z };
		glm::vec3 Button = { colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, colors[ImGuiCol_Button].z };
		glm::vec3 FrameBg = { colors[ImGuiCol_FrameBg].x, colors[ImGuiCol_FrameBg].y, colors[ImGuiCol_FrameBg].z };
		glm::vec3 Tab = { colors[ImGuiCol_Tab].x, colors[ImGuiCol_Tab].y, colors[ImGuiCol_Tab].z };
		glm::vec3 TitleBg = { colors[ImGuiCol_TitleBg].x, colors[ImGuiCol_TitleBg].y, colors[ImGuiCol_TitleBg].z };

		ImGui::Begin("UI colour scheme");
		ImGui::ColorEdit3("Window Bg", glm::value_ptr(WindowBg));
		ImGui::ColorEdit3("Header", glm::value_ptr(Header));
		ImGui::ColorEdit3("Button", glm::value_ptr(Button));
		ImGui::ColorEdit3("Frame Bg", glm::value_ptr(FrameBg));
		ImGui::ColorEdit3("Tab", glm::value_ptr(Tab));
		ImGui::ColorEdit3("Title Bg", glm::value_ptr(TitleBg));
		ImGui::End();

		colors[ImGuiCol_WindowBg] = ImVec4(WindowBg.x, WindowBg.y, WindowBg.z, 1.0);
		colors[ImGuiCol_Header] = ImVec4(Header.x, Header.y, Header.z, 1.0);
		colors[ImGuiCol_Button] = ImVec4(Button.x, Button.y, Button.z, 1.0);
		colors[ImGuiCol_FrameBg] = ImVec4(FrameBg.x, FrameBg.y, FrameBg.z, 1.0);
		colors[ImGuiCol_Tab] = ImVec4(Tab.x, Tab.y, Tab.z, 1.0);
		colors[ImGuiCol_TitleBg] = ImVec4(TitleBg.x, TitleBg.y, TitleBg.z, 1.0);

	}

	void EditorLayer::UI_NewProject()
	{
		ImGui::OpenPopup("ProjectPopup");

		ImVec2 viewportCenter = ImGui::GetMainViewport()->GetWorkCenter();
		glm::vec2 center = { viewportCenter.x, viewportCenter.y };
		glm::vec2 size = { 400.0f, 300.0f };
		center -= size;
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);
		ImGui::SetNextWindowSize({ size.x, size.y });
		ImGui::SetNextWindowPos({ center.x, center.y });
		if (ImGui::BeginPopupModal("ProjectPopup", &m_DisplayProjectPopup, ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("New Projects isn't implemented yet");
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
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
					if (control)
					{
						ScriptEngine::ReloadAssembly();
						break;
					}
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
					break;
			}
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		/*if (e.GetMouseButton() == Mouse::ButtonLeft && m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);*/

		return false;
	}

	Entity EditorLayer::GetHoveredEntity()
	{
		Entity hoveredEntity;
		if (m_MouseX >= 0 && m_MouseY >= 0 && m_MouseX < (int)m_ViewportSize.x && m_MouseY < (int)m_ViewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, m_MouseX, m_MouseY);
			hoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_EditorScene.get());
		}

		return (int)hoveredEntity > -1 ? hoveredEntity : Entity();
	}

	void EditorLayer::NewProject()
	{
		OE_WARN("Projects are not fully developed."); // TODO: <- do this
		// Project::New();

		m_DisplayProjectPopup = true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& filepath)
	{
		if (Project::Load(filepath))
		{
			ContentBrowserPanel::Init();
			AssetHandle startScene = Project::GetActive()->GetConfig().StartScene;
			if (startScene)
				OpenScene(startScene);
		}
	}

	void EditorLayer::SaveProject()
	{
		// Project::SaveActive();
	}

	void EditorLayer::NewScene(const std::string& filepath)
	{
		std::vector<Material> materials = m_EditorScene->GetMaterials();
		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->SetFilepath(filepath);
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorScene->SetMaterials(materials);
		SceneHierarchyPanel::SetContext(m_EditorScene);
		MaterialPanel::SetContext(m_EditorScene);
	}

	void EditorLayer::OpenScene()
	{
		/*std::string filepath = FileDialogs::OpenFile("OpenEngine Scene (*.openengine)\0*.openengine\0");
		if (!filepath.empty())
			OpenScene(filepath);*/
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
		Ref<Scene> newScene = CreateRef<Scene>();
		readOnlyScene->CopyTo(newScene);

		m_EditorScene = newScene;
		SceneHierarchyPanel::SetContext(m_EditorScene);
		MaterialPanel::SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
		auto filepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetAssetManager()->GetFilePath(handle);
		m_EditorScene->SetFilepath(filepath);
	}

	void EditorLayer::SaveScene()
	{
		const std::string filepath = m_EditorScene->GetFilepath().string();
		if (filepath != "UntitledScene.openengine")
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filepath);
			ContentBrowserPanel::LoadAssets();
			return;
		}
		SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("OpenEngine Scene (*.openengine)\0*.openengine\0");
		if (!path.empty())
		{
			std::filesystem::path filepath(path);
			m_EditorScene->SetFilepath(filepath.lexically_relative(Project::GetActiveAssetDirectory()));
			SaveScene();
		}
	}

	void EditorLayer::RenderOverlay()
	{
		RenderCommand::DisableDepthTest();

		// TODO: move to private variable to have customize option inside the editor
		glm::vec4 staticBodyColor = { 0.2f, 0.3f, 1.0f, 1.0f };
		glm::vec4 dynamicBodyColor = { 0.0f, 1.0f, 0.0f, 1.0f };

		if (m_SceneState == SceneState::Edit)
		{
			Renderer2D::BeginScene(m_EditorCamera);
			
			if (m_DisplayPhysicsColliders)
			{
				{
					auto view = m_EditorScene->GetAllEntitiesWith<TransformComponent, CircleColider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, cc2d] = view.get<TransformComponent, CircleColider2DComponent>(entity);
						glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);
						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::scale(glm::mat4(1.0f), scale);

						glm::vec4 color;
						Entity e = { entity, m_EditorScene.get() };
						if (e.HasComponent<RigidBody2DComponent>())
							color = e.GetComponent<RigidBody2DComponent>().Type == RigidBody2DComponent::BodyType::Static ? staticBodyColor : dynamicBodyColor;
						Renderer2D::DrawCircle(transform, color, 0.05f);
					}
				}

				{
					auto view = m_EditorScene->GetAllEntitiesWith<TransformComponent, BoxColider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, bc2d] = view.get<TransformComponent, BoxColider2DComponent>(entity);
						glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);
						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
							* glm::scale(glm::mat4(1.0f), scale);

						glm::vec4 color;
						Entity e = { entity, m_EditorScene.get() };
						if (e.HasComponent<RigidBody2DComponent>())
							color = e.GetComponent<RigidBody2DComponent>().Type == RigidBody2DComponent::BodyType::Static ? staticBodyColor : dynamicBodyColor;
						Renderer2D::DrawRect(transform, color);
					}
				}
			}
		}
		else
		{
			Entity camera = m_RuntimeScene->PrimaryCamera();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetTransform());

			if (m_DisplayPhysicsColliders)
			{
				{
					auto view = m_RuntimeScene->GetAllEntitiesWith<TransformComponent, CircleColider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, cc2d] = view.get<TransformComponent, CircleColider2DComponent>(entity);
						glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);
						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::scale(glm::mat4(1.0f), scale);

						glm::vec4 color;
						Entity e = { entity, m_RuntimeScene.get() };
						if (e.HasComponent<RigidBody2DComponent>())
							color = e.GetComponent<RigidBody2DComponent>().Type == RigidBody2DComponent::BodyType::Static ? staticBodyColor : dynamicBodyColor;
						Renderer2D::DrawCircle(transform, color, 0.05f);
					}
				}

				{
					auto view = m_RuntimeScene->GetAllEntitiesWith<TransformComponent, BoxColider2DComponent>();
					for (auto entity : view)
					{
						auto [tc, bc2d] = view.get<TransformComponent, BoxColider2DComponent>(entity);
						glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
						glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);
						glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
							* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
							* glm::scale(glm::mat4(1.0f), scale);

						glm::vec4 color;
						Entity e = { entity, m_RuntimeScene.get() };
						if (e.HasComponent<RigidBody2DComponent>())
							color = e.GetComponent<RigidBody2DComponent>().Type == RigidBody2DComponent::BodyType::Static ? staticBodyColor : dynamicBodyColor;
						Renderer2D::DrawRect(transform, color);
					}
				}
			}
		}

		Renderer2D::EndScene();

		RenderCommand::EnableDepthTest();

	}

}
