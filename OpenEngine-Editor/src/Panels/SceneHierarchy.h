#pragma once

#include "Panel.h"

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Core/Log.h"
#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scene/Entity.h"
#include "OpenEngine/Renderer/Renderer2D.h"

namespace OpenEngine {

	class SceneHierarchyPanel : public Panel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void DisplaySceneHierarchy(bool display) { m_DisplaySceneHierarchy = display; }
		void DisplayProperties(bool display) { m_DisplayProperties = display; }

		static std::string GetName() { return "SceneHierarchyPanel"; }
		void OnImGuiRender() override;

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

		void SetSceneState(SceneState state) { m_SceneState = state; }
		SceneState GetSceneState() { return m_SceneState; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		bool m_DisplaySceneHierarchy = true;
		bool m_DisplayProperties = true;

		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		Ref<Texture2D> m_ClearTextureImage;

		SceneState m_SceneState = SceneState::Edit;
	};

}
