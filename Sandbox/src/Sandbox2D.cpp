#include "Sandbox2D.h"
#include "OpenEngine/Renderer/Renderer2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	OE_PROFILE_FUNCTION();

	m_CheckerboardTexture = OpenEngine::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	OE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(OpenEngine::Timestep ts)
{
	OE_PROFILE_FUNCTION();

	//Update
	m_CameraController.OnUpdate(ts);
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		OpenEngine::Quad quad1 = { { 0.5f, -0.25f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor };
		OpenEngine::Quad quad2 = { { -0.5f, 0.25f, 0.0f }, { 1.0f, 0.5f }, m_Square2Color, rotation, 1.0f };
		OpenEngine::Quad quad3;
		OpenEngine::Quad quad4;

		quad3.position = { 0.0f, 0.0f, -0.1f };
		quad3.size = { 10.0f, 10.0f };
		quad3.texture = m_CheckerboardTexture;
		quad3.scale = 10.0f;

		quad4.position = { 0.0f, 0.0f, 0.0f };
		quad4.size = { 1.0f, 1.0f };
		quad4.texture = m_CheckerboardTexture;
		quad4.rotation = 45.0f;
		quad4.scale = 20.0f;


		//Render
		OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		OpenEngine::RenderCommand::Clear();

		OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		OpenEngine::Renderer2D::DrawQuad(quad1);
		OpenEngine::Renderer2D::DrawQuad(quad2);
		//OpenEngine::Renderer2D::DrawRotatedQuad({ 0.5f, -0.5f }, { 1.0f, 0.75f }, glm::radians(45.0f), m_Square2Color);
		OpenEngine::Renderer2D::DrawQuad(quad3);
		OpenEngine::Renderer2D::DrawQuad(quad4);
		OpenEngine::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	OE_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::ColorEdit4("Square2 Color", glm::value_ptr(m_Square2Color));
	ImGui::End();
}

void Sandbox2D::OnEvent(OpenEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
