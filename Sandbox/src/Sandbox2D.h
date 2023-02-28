#pragma once

#include "OpenEngine.h"

class Sandbox2D : public OpenEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(OpenEngine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(OpenEngine::Event& e) override;
private:

	OpenEngine::OrthographicCameraController m_CameraController;
	OpenEngine::Ref<OpenEngine::VertexArray> m_SquareVA;
	OpenEngine::Ref<OpenEngine::Shader> m_Shader;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};