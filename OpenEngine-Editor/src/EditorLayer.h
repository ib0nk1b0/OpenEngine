#pragma once

#include "OpenEngine.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialPanel.h"
#include "Panels/AssetManagerPanel.h"

#include "OpenEngine/Scene/SceneSerializer.h"
#include "OpenEngine/Serialization/Serializer.h"
#include "OpenEngine/Renderer/EditorCamera.h"

namespace OpenEngine {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		void ScenePlay();
		void SceneStop();
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		Entity GetHoveredEntity();

		void NewProject();
		void OpenProject();
		void OpenProject(const std::filesystem::path& filepath);
		void SaveProject();

		void NewScene(const std::string& filepath = "UntitledScene.openengine");
		void OpenScene();
		void OpenScene(AssetHandle handle);
		void SaveScene();
		void SaveSceneAs();

		void RenderOverlay();

		// UI panles
		void UI_MenuBar();
		void UI_Stats();
		void UI_EditorCameraPanel();
		void UI_Toolbar();
		void UI_ColorScheme();
		void UI_NewProject();
	private:
		Timestep m_FrameTime;

		bool m_DisplayStats = true;
		bool m_DisplayEditorCameraUI = true;
		bool m_DisplayMaterials = true;
		bool m_DisplayPhysicsColliders = false;
		bool m_DisplayProjectPopup = false;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		Ref<Scene> m_RuntimeScene;

		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;

		Entity m_HoveredEntity;
		int m_MouseX, m_MouseY;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 1.0f };
		glm::vec2 m_ViewportBounds[2];

		bool m_CursorEnabled = true;

		int m_GizmoType = -1;

		Ref<Texture2D> m_PlayIcon, m_StopIcon;
		SceneState m_SceneState = SceneState::Edit;
	};

}
