#pragma once

#include "OpenEngine.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	class Player : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;
		void Move(Timestep ts);
		virtual void OnDestroy() override;

		glm::quat GetOrientation();
		glm::vec3 GetForwardDirection();
		glm::vec3 GetRightDirection();
	private:
		float m_Speed = 2.0f;
	};

}