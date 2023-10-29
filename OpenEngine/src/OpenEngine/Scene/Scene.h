#pragma once

#include "OpenEngine/Core/Timestep.h"
#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Renderer/EditorCamera.h"
#include "OpenEngine/Renderer/Renderer.h"

#include "entt.hpp"

class b2World;

namespace OpenEngine {

	class Entity;
	enum class SceneState
	{
		Edit = 0, Play
	};

	class Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		Entity DuplicateEntity(Entity other);
		Entity GetEntityByUUID(UUID uuid);
		Entity GetEntityByName(const std::string& name);

		void OnEditorStart();
		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdate(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		std::string GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::string& filepath) { m_Filepath = filepath; }

		void CopyTo(Ref<Scene> other); 

		void ToggleCursor();
		void ToggleGrid() { m_GridEnabled = !m_GridEnabled; };
		void SetGridSize(int gridSize) { m_GridSize = gridSize; }
		int GetGridSize() { return m_GridSize; }

		std::vector<entt::entity> GetEntitiesWithParents();
		std::vector<Material> GetMaterials() { return m_Materials; }
		void SetMaterials(std::vector<Material> materials) { m_Materials = materials; }
	// TODO: Remove
	public:
		bool m_RuntimeActive = false;
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_Filepath;

		bool m_CursorEnabled = true;
		bool m_GridEnabled = true;
		int m_GridSize = 8;

		std::vector<Material> m_Materials;
		std::vector<Mesh> m_Meshes;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class MaterialPanel;
	};

}