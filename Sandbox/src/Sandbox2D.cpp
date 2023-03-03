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

	//Render
	OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	OpenEngine::RenderCommand::Clear();

	OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	OpenEngine::Renderer2D::DrawQuad({ -0.5f, 0.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor);
	OpenEngine::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.5f }, m_Square2Color);
	//OpenEngine::Renderer2D::DrawRotatedQuad({ 0.5f, -0.5f }, { 1.0f, 0.75f }, glm::radians(45.0f), m_Square2Color);
	//OpenEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
	OpenEngine::Renderer2D::EndScene();
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
