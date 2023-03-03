#include "GameLayer.h"

#include "OpenEngine/Renderer/Renderer2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameLayer::GameLayer()
	: Layer("GameLayer"), m_CameraController(1280.0f / 720.0f, false, false), 
	m_PaddleController1({ -1.65f, 0.0f }, { 0.03f, 0.4f }, { OE_KEY_W, OE_KEY_S }),
	m_PaddleController2({ 1.65f, 0.0f }, { 0.03f, 0.4f }, { OE_KEY_UP, OE_KEY_DOWN }),
	m_Ball({ 0.0f, 0.0f }, { -0.5f, -0.5f }, { 0.04f, 0.04f })
{
}

void GameLayer::OnAttach()
{
	m_CheckerboardTexture = OpenEngine::Texture2D::Create("assets/textures/Checkerboard.png");
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(OpenEngine::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	m_PaddleController1.OnUpdate(ts);
	m_PaddleController2.OnUpdate(ts);
	m_Ball.OnUpdate(ts);

	float ballX = m_Ball.GetPosition().x;
	float ballY = m_Ball.GetPosition().y;
	float ballW = m_Ball.GetSize().x;
	float ballH = m_Ball.GetSize().y;


	float paddleX = m_PaddleController1.GetPaddle().GetPosition().x;
	float paddleY = m_PaddleController1.GetPaddle().GetPosition().y;
	float paddleW = m_PaddleController1.GetPaddle().GetSize().x;
	float paddleH = m_PaddleController1.GetPaddle().GetSize().y;

	if (paddleX < ballX + ballW &&
		paddleX + paddleW > ballX &&
		paddleY < ballY + ballH &&
		paddleY + paddleH > ballY)
	{
		m_Ball.SetVelocity({ -m_Ball.GetVelocity().x, m_Ball.GetVelocity().y });
	}

	paddleX = m_PaddleController2.GetPaddle().GetPosition().x;
	paddleY = m_PaddleController2.GetPaddle().GetPosition().y;
	paddleW = m_PaddleController2.GetPaddle().GetSize().x;
	paddleH = m_PaddleController2.GetPaddle().GetSize().y;

	if (paddleX < ballX + ballW &&
		paddleX + paddleW > ballX &&
		paddleY < ballY + ballH &&
		paddleY + paddleH > ballY)
	{
		m_Ball.SetVelocity({ -m_Ball.GetVelocity().x, m_Ball.GetVelocity().y });
	}

	OpenEngine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	OpenEngine::RenderCommand::Clear();

	OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	OpenEngine::Renderer2D::DrawQuad(m_PaddleController1.GetPaddle().GetPosition(), m_PaddleController1.GetPaddle().GetSize());
	OpenEngine::Renderer2D::DrawQuad(m_PaddleController2.GetPaddle().GetPosition(), m_PaddleController2.GetPaddle().GetSize());
	OpenEngine::Renderer2D::DrawQuad(m_Ball.GetPosition(), m_Ball.GetSize());
	OpenEngine::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
}

void GameLayer::OnEvent(OpenEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}

