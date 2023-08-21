#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Physics/World.h"

namespace OpenEngine {

	class Physics2D
	{
	public:
		Physics2D(float gravity);

		void AddWorld(const World2D& world) { m_World = world; m_WorldAdded = true; }

		void Update();
	private:
		float m_Gravity;
		bool m_WorldAdded = false;
		World2D m_World;
	};

}