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
	Paddle(const glm::vec3& position);

	glm::vec2& GetSize() { return m_Quad.size; }

	glm::vec3& GetPosition() { return m_Quad.position; }
	void SetPosition(const glm::vec3& position) { m_Quad.position = position; }

	OpenEngine::Quad GetQuad() { return m_Quad; }

	int GetScore() { return m_Score; }
	void AddScore() { m_Score++; }

	void Draw();
private:
	OpenEngine::Quad m_Quad;
	glm::vec3 m_Position;
	int m_Score = 0;
};

class PaddleController
{
public:
	PaddleController(const glm::vec3& position, Keybinds keybinds);

	void OnUpdate(OpenEngine::Timestep ts);
	void OnEvent(OpenEngine::Event& e);

	Paddle& GetPaddle() { return m_Paddle; }

private:
	Paddle m_Paddle;
	glm::vec3 m_PaddlePosition;
	float m_PaddleSpeed = 1.0f;
	Keybinds m_Keybinds;
};