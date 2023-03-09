#include "GameLayer.h"

#include "OpenEngine/Renderer/Renderer2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameLayer::GameLayer()
	: Layer("GameLayer"), m_CameraController(1280.0f / 720.0f, false, false), 
	m_PaddleController1({ -1.65f, 0.0f, 0.0f }, { OE_KEY_W, OE_KEY_S }),
	m_PaddleController2({ 1.65f, 0.0f, 0.0f }, { OE_KEY_UP, OE_KEY_DOWN }),
	m_Ball({ -1.0f, -1.0f })
{
}

void GameLayer::OnAttach()
{
	m_CheckerboardTexture = OpenEngine::Texture2D::Create("assets/textures/Checkerboard.png");
}

void GameLayer::OnDetach()
{
}

bool GameLayer::CollisionRect(OpenEngine::Quad quad1, OpenEngine::Quad quad2)
{
	if (quad1.position.x < quad2.position.x + quad2.size.x / 2 &&
		quad1.position.x + quad1.size.x / 2 > quad2.position.x &&
		quad1.position.y < quad2.position.y + quad2.size.y / 2 &&
		quad1.position.y + quad1.size.y / 2> quad2.position.y)
	{
		return true;
	}
	else return false;
}

void GameLayer::OnUpdate(OpenEngine::Timestep ts)
{
	OpenEngine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	OpenEngine::RenderCommand::Clear();

	OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());

	OpenEngine::Quad centerQuad;
	centerQuad.position = { 0.0f, 0.95f, 0.0f };
	centerQuad.size = { 0.005f, 0.05f};

	for (int i = 0; i < 15; i++)
	{
		OpenEngine::Renderer2D::DrawQuad(centerQuad);
		centerQuad.position.y -= 0.15;
	}

	if (m_Ball.GameReset)
	{
		if (m_Ball.GetPosition().x <= 1.0f / 9.0f * 16.0f)
			m_PaddleController2.GetPaddle().AddScore();
		else
			m_PaddleController1.GetPaddle().AddScore();

		m_Ball.SetPosition({ -0.02f, -0.02f, 0.0f });
		m_Ball.GameReset = false;
	}

	m_CameraController.OnUpdate(ts);

	m_PaddleController1.OnUpdate(ts);
	m_PaddleController2.OnUpdate(ts);
	m_Ball.OnUpdate(ts);

	if (CollisionRect(m_PaddleController1.GetPaddle().GetQuad(), m_Ball.GetQuad()) || CollisionRect(m_PaddleController2.GetPaddle().GetQuad(), m_Ball.GetQuad()))
	{
		m_Ball.SetVelocity({ -m_Ball.GetVelocity().x, m_Ball.GetVelocity().y });
	}

	OpenEngine::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("Score");
	ImGui::Text("%i", m_PaddleController1.GetPaddle().GetScore());
	ImGui::Text("%i", m_PaddleController2.GetPaddle().GetScore());
	ImGui::End();
}

void GameLayer::OnEvent(OpenEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}

