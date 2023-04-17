#include "oepch.h"
#include "Entity.h"

namespace OpenEngine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene) 
	{
	}

}