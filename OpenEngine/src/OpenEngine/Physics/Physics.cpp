#include "oepch.h"
#include "OpenEngine/Physics/Physics.h"

namespace OpenEngine {

	Physics2D::Physics2D(float gravity)
		: m_Gravity(gravity) {}

	void Physics2D::Update()
	{
		if (m_WorldAdded)
			m_World.Update();
	}

}