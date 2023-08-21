#pragma once

#include "OpenEngine/Core/Timestep.h"
#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Renderer/EditorCamera.h"

#include "entt.hpp"

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

		void OnRuntimeStart();

		void OnUpdate(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		std::string GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::string& filepath) { m_Filepath = filepath; }

		void CopyTo(Ref<Scene> other); 

		std::vector<entt::entity> GetEntitiesWithParents();

	// TODO: Remove
	public:
		bool m_RuntimeActive = false;
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::string m_Filepath;

		friend class Entity;
		friend class Serializer;
		friend class SceneHierarchyPanel;
	};

}