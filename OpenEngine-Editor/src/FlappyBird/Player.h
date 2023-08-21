#pragma once

#include "OpenEngine.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	class Player : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnDestroy() override;
	private:
		glm::vec2 m_Velocity{ 2.0f, 0.0f };
		float m_Power = 6.5f;
		float m_Gravity = 0.03f;
	};

}