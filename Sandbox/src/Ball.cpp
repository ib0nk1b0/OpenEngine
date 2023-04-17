#include "Ball.h"

Ball::Ball(const glm::vec2& vel)
	: m_Velocity(vel)
{
	m_Quad.position = { 0.0f, 0.0f, 0.0f };
	m_Quad.size = { 0.04f, 0.04f };
}

void Ball::OnUpdate(OpenEngine::Timestep ts)
{
	m_Quad.position.x += m_Velocity.x * ts;
	m_Quad.position.y += m_Velocity.y * ts;

	float windowWidth = 1.0f / 9.0f * 16.0f;

	if (m_Quad.position.x <= -windowWidth || m_Quad.position.x >= windowWidth)
	{
		GameReset = true;
		//Add a way to get the player from the game class
	}

	if (m_Quad.position.y >= 1.0f - m_Quad.size.y || m_Quad.position.y <= -1.0f)
		m_Velocity.y = -m_Velocity.y;

	//OpenEngine::Renderer2D::DrawQuad(m_Quad);
}
