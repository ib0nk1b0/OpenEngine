#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Core/Log.h"
#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scene/Entity.h"

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
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		bool m_DisplaySceneHierarchy = true;
		bool m_DisplayProperties = true;

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
