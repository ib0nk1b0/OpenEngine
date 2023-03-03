#pragma once

#include "OpenEngine.h"
#include "OpenEngine/Core/Timestep.h"
#include <glm/glm.hpp>

struct Keybinds
{
	int Up;
	int Down;
};

class Paddle
{
public:
	Paddle(const glm::vec2& position, const glm::vec2& size);

	glm::vec2& GetSize() { return m_Size; }
	glm::vec2& GetPosition() { return m_Position; }
	void SetPosition(const glm::vec2& position) { m_Position = position; }

	int GetScore() { return m_Score; }
	void AddScore() { m_Score++; }
private:
	glm::vec2 m_Position = { 0.0f, 0.0f };
	glm::vec2 m_Size;
	int m_Score;
};

class PaddleController
{
public:
	PaddleController(const glm::vec2& position, const glm::vec2& size, Keybinds keybinds);

	void OnUpdate(OpenEngine::Timestep ts);
	void OnEvent(OpenEngine::Event& e);

	Paddle& GetPaddle() { return m_Paddle; }

private:
	Paddle m_Paddle;
	glm::vec2 m_PaddlePosition;
	float m_PaddleSpeed = 0.5f;
	Keybinds m_Keybinds;
};