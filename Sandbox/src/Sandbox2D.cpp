#include "Sandbox2D.h"
#include "OpenEngine/Renderer/Renderer2D.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
{
}

void Sandbox2D::OnAttach()
{
	OE_PROFILE_FUNCTION();
}

void Sandbox2D::OnDetach()
{
	OE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(OpenEngine::Timestep ts)
{
	OE_PROFILE_FUNCTION();


#if 0
	//Update
	m_CameraController.OnUpdate(ts);
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;
		//Render
		OpenEngine::Renderer2D::ResetStats();
		OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		OpenEngine::RenderCommand::Clear();

		OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.8f };
				OpenEngine::Renderer2D::DrawQuad({ x, y, -0.1f }, { 0.45f, 0.45f }, color);
			}
		}
		
		OpenEngine::Renderer2D::DrawCircle(glm::mat4(1.0f), glm::vec4(1, 0, 1, 1));

		OpenEngine::Renderer2D::EndScene();
	}
#endif
}

void Sandbox2D::OnEvent(OpenEngine::Event& e)
{
	//m_CameraController.OnEvent(e);
}
