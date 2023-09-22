#pragma once

#include "Entity.h"

namespace OpenEngine {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		glm::vec3& GetTranslation()
		{
			auto& translation = m_Entity.GetTranslation();
			return translation;
		}

		Entity FindEntityByName(const std::string& name)
		{
			return m_Entity.FindEntityByName(name);
		}

		Entity CreateEntity(const std::string& name)
		{
			return m_Entity.m_Scene->CreateEntity(name);
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}
	private:
		Entity m_Entity;

		friend class Scene;
	};

}