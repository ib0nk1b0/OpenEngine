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
		//TransformComponent& GetTransform() { return GetComponent<TransformComponent>(); }
		glm::mat4 GetTransform() { return GetComponent<TransformComponent>().GetTransform(); }
		glm::vec3& GetTranslation() { return GetComponent<TransformComponent>().Translation; }
		glm::vec3& GetScale() { return GetComponent<TransformComponent>().Scale; }
		glm::vec3& GetRotation() { return GetComponent<TransformComponent>().Rotation; }
		std::string GetName() { return GetComponent<TagComponent>().Tag; }

		Entity FindEntityByName(const std::string& name) { return m_Scene->GetEntityByName(name); }
		
		bool HasParent()
		{
			return GetComponent<ParentComponent>().ParentID != 0;
		}

		bool HasChildren()
		{
			return GetComponent<ParentComponent>().Children.size() > 0;
		}

		Entity GetParent()
		{
			return m_Scene->GetEntityByUUID(GetComponent<ParentComponent>().ParentID);
		}

		void SetParent(Entity parent)
		{
			if (!HasComponent<ParentComponent>())
				AddComponent<ParentComponent>();
			auto& parentComponent = GetComponent<ParentComponent>();
			parentComponent.ParentID = parent.GetUUID();
			parent.GetComponent<ParentComponent>().Children.push_back(GetUUID());
		}

		void RemoveParent()
		{
			Entity parent = GetParent();
			
			auto& children = parent.GetComponent<ParentComponent>().Children;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i] == GetUUID())
					children.erase(children.begin() + i);
			}

			GetComponent<ParentComponent>().ParentID = 0;
		}

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class ScriptableEntity;
	};

}