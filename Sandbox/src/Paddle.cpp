#include "Paddle.h"

Paddle::Paddle(const glm::vec3& position)
	: m_Position(position)
{
	m_Quad.position = m_Position;
	m_Quad.size = { 0.03f, 0.4f };
	m_Score = 0;
}

void Paddle::Draw()
{
	//OpenEngine::Renderer2D::DrawQuad(m_Quad);
}

PaddleController::PaddleController(const glm::vec3& position, Keybinds keybinds)
	: m_Paddle(position), m_Keybinds(keybinds), m_PaddlePosition(position)
{
}

void PaddleController::OnUpdate(OpenEngine::Timestep ts)
{
	if (OpenEngine::Input::IsKeyPressed(m_Keybinds.Up))
		m_PaddlePosition.y += m_PaddleSpeed * ts;
	if (OpenEngine::Input::IsKeyPressed(m_Keybinds.Down))
		m_PaddlePosition.y -= m_PaddleSpeed * ts;

	m_Paddle.SetPosition(m_PaddlePosition);
	m_Paddle.Draw();
}

void PaddleController::OnEvent(OpenEngine::Event& e)
{
}
