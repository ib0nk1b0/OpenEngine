#include "Ball.h"

Ball::Ball(const glm::vec2& pos, const glm::vec2& vel, const glm::vec2& size)
	: m_Position(pos), m_Velocity(vel), m_Size(size)
{
}

void Ball::OnUpdate(OpenEngine::Timestep ts)
{
	m_Position.x += m_Velocity.x * ts;
	m_Position.y += m_Velocity.y * ts;

	float num = 1.0f / 9.0f * 16.0f;

	if (m_Position.x <= -num || m_Position.x >= num)
	{
		m_Position = { 0.0f, 0.0f };
		//Add a way to get the player from the game class
	}

	if (m_Position.y >= 1.0f || m_Position.y <= -1.0f)
		m_Velocity.y = -m_Velocity.y;
}
