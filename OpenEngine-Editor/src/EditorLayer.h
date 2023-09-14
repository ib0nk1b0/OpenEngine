#pragma once

#include "OpenEngine.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MaterialPanel.h"

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

		void NewProject();
		void NewScene(const std::string& filepath = "UntitledScene.openengine");
		void OpenScene();
		void OpenScene(const std::filesystem::path& filepath);
		void SaveScene();
		void SaveSceneAs();

		// UI panles
		void UI_MenuBar();
		void UI_Stats();
		void UI_EditorCameraPanel();
		void UI_Toolbar();
	private:
		Timestep m_FrameTime;

		bool m_DisplayStats = true;
		bool m_DisplayEditorCameraUI = true;
		bool m_DisplaySceneHierarchy = true;
		bool m_DisplayProperties = true;
		bool m_DisplayMaterials = true;

		Ref<Scene> m_EditorScene;
		Ref<Scene> m_RuntimeScene;
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;

		Entity m_HoveredEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 1.0f };
		glm::vec2 m_ViewportBounds[2];

		bool m_CursorEnabled = true;

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		MaterialPanel m_MaterialPanel;

		Ref<Texture2D> m_PlayIcon, m_StopIcon;
		SceneState m_SceneState = SceneState::Edit;
	};

}
