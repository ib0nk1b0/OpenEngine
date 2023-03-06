#pragma once

#include "OpenEngine.h"

#include <glm/glm.hpp>

class Ball
{
public:
	Ball(const glm::vec2& vel);

	glm::vec3& GetPosition() { return m_Quad.position; }
	void SetPosition(const glm::vec3& position) { m_Quad.position = position; }

	glm::vec2& GetSize() { return m_Quad.size; }

	glm::vec2& GetVelocity() { return m_Velocity; }
	void SetVelocity(const glm::vec2& velocity) { m_Velocity = velocity; }

	OpenEngine::Quad GetQuad() { return m_Quad; }

	void OnUpdate(OpenEngine::Timestep ts);
public:
	bool GameReset = false;
private:
	OpenEngine::Quad m_Quad;
	glm::vec2 m_Velocity;
};