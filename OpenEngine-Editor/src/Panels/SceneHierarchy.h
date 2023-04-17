#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Core/Log.h"
#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scene/Entity.h"
#include "OpenEngine/Renderer/Renderer2D.h"

namespace OpenEngine {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void DisplaySceneHierarchy(bool display) { m_DisplaySceneHierarchy = display; }
		void DisplayProperties(bool display) { m_DisplayProperties = display; }

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		bool m_DisplaySceneHierarchy = true;
		bool m_DisplayProperties = true;

		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		Ref<Texture2D> m_ClearTextureImage;
	};

}
