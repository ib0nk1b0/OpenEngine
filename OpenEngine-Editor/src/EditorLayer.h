#pragma once

#include "OpenEngine.h"

#include "Panels/SceneHierarchy.h"

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
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewProject();
		void NewScene(const std::string& filepath = "UntitledScene.openengine");
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();

		// UI panles
		void UI_MenuBar();
		void UI_Stats();
		void UI_EditorCameraPanel();
		void UI_Toolbar();
	private:
		bool m_DisplayStats = true;
		bool m_DisplayEditorCameraUI = true;
		bool m_DisplaySceneHierarchy = true;
		bool m_DisplayProperties = true;

		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 1.0f };

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

		Ref<Texture2D> m_PlayIcon, m_StopIcon;
	};

}
