#pragma once

#include "OpenEngine/Scene/Components.h"

namespace OpenEngine {

	class World2D
	{
	public:
		World2D();

		void Add(RigidBody2DComponent rigidBodies) { m_RigidBodies.push_back(rigidBodies); }

		void Update();
	private:
		std::vector<RigidBody2DComponent> m_RigidBodies;
	};

}