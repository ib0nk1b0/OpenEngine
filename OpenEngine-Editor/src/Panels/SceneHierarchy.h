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
		static void Init();

		static void SetContext(const Ref<Scene>& scene);
		static void ToggleSceneHierarchy() { m_DisplaySceneHierarchy = !m_DisplaySceneHierarchy; }
		static void ToggleProperties() { m_DisplayProperties = !m_DisplayProperties; }

		static void OnImGuiRender();

		static Entity GetSelectedEntity() { return m_SelectionContext; }
		static void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

		static bool DisplaySceneHirerarchy() { return m_DisplaySceneHierarchy; }
		static bool DisplayProperties() { return m_DisplayProperties; }
	private:
		static void DrawEntityNode(Entity entity);
		static void DrawComponents(Entity entity);
	private:
		inline static bool m_DisplaySceneHierarchy;
		inline static bool m_DisplayProperties;

		inline static Ref<Scene> m_Context;
		inline static Entity m_SelectionContext;

		inline static Ref<Texture2D> m_ClearTextureImage;
	};

}
