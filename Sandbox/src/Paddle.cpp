#include "Paddle.h"

Paddle::Paddle(const glm::vec2& position, const glm::vec2& size)
	: m_Position(position), m_Size(size)
{
}

PaddleController::PaddleController(const glm::vec2& position, const glm::vec2& size, Keybinds keybinds)
	: m_Paddle(position, size), m_Keybinds(keybinds), m_PaddlePosition(position)
{

}

void PaddleController::OnUpdate(OpenEngine::Timestep ts)
{
	if (OpenEngine::Input::IsKeyPressed(m_Keybinds.Up))
		m_PaddlePosition.y += m_PaddleSpeed * ts;
	if (OpenEngine::Input::IsKeyPressed(m_Keybinds.Down))
		m_PaddlePosition.y -= m_PaddleSpeed * ts;

	m_Paddle.SetPosition(m_PaddlePosition);
}

void PaddleController::OnEvent(OpenEngine::Event& e)
{
}
