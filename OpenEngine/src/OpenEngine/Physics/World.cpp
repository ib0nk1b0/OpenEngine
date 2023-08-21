#include "oepch.h"
#include "OpenEngine/Physics/World.h"

namespace OpenEngine {

	World2D::World2D()
	{
		
	}

	void World2D::Update()
	{
		for (auto& rigidBody : m_RigidBodies)
		{
			if (rigidBody.Type == BodyType::Static)
				OE_CORE_INFO("Static");
		}
	}

}