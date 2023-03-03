#pragma once

#include "OpenEngine.h"

#include <glm/glm.hpp>

class Ball
{
public:
	Ball(const glm::vec2& pos, const glm::vec2& vel, const glm::vec2& size);

	glm::vec2& GetPosition() { return m_Position; }
	glm::vec2& GetSize() { return m_Size; }

	glm::vec2& GetVelocity() { return m_Velocity; }
	void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }

	void OnUpdate(OpenEngine::Timestep ts);
private:
	glm::vec2 m_Position;
	glm::vec2 m_Velocity;
	glm::vec2 m_Size;
};