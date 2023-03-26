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
		void DrawEntities();
		bool OnKeyPressed(KeyPressedEvent& e);
		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		Ref<Scene> m_ActiveScene;

		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_Shader;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_OpenEngineTexture;

		Entity m_MainCamera;
		Entity m_SecondCamera;

		bool m_MainCameraPrimary = true;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize{ 1.0f };

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec4 m_Square2Color = { 0.8f, 0.2f, 0.3f, 1.0f };

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}
