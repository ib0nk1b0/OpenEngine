#pragma once

#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scene/Components.h"

#include <entt.hpp>

namespace OpenEngine {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't contain the component.");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			OE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't contain the component.");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		glm::mat4 GetTransform() { return GetComponent<TransformComponent>().GetTransform(); }
		glm::vec3 GetTranslation() { return GetComponent<TransformComponent>().Translation; }
		glm::vec3 GetScale() { return GetComponent<TransformComponent>().Scale; }
		glm::vec3 GetRotation() { return GetComponent<TransformComponent>().Rotation; }
		std::string GetName() { return GetComponent<TagComponent>().Tag; }
		bool HasParent()
		{
			return !GetComponent<ParentComponent>().ParentName.empty();
		}

		Entity FindEntityByName(const std::string& name) { return m_Scene->GetEntityByName(name); }

		void SetParent(Entity parent)
		{
			if (!HasComponent<ParentComponent>())
				AddComponent<ParentComponent>();
			auto& parentComponent = GetComponent<ParentComponent>();
			parentComponent.ParentName = parent.GetName();
			parentComponent.ParentID = parent.GetUUID();
		}

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};

}