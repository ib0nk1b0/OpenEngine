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

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
