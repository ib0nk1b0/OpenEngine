#include "Sandbox2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenEngine/Renderer/Renderer2D.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(OpenEngine::Timestep ts)
{
	//Update
	m_CameraController.OnUpdate(ts);

	//Render
	OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	OpenEngine::RenderCommand::Clear();

	OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
	OpenEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_SquareColor);
	OpenEngine::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(OpenEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
