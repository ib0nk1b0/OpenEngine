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
		void NewScene(const std::string& filepath = "UntitledScene.openengine");
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();
	private:

		Ref<Scene> m_ActiveScene;
		Ref<Framebuffer> m_Framebuffer;
		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 1.0f };

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}
