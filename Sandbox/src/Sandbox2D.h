#pragma once

#include "OpenEngine.h"

class Sandbox2D : public OpenEngine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(OpenEngine::Timestep ts) override;
	void OnEvent(OpenEngine::Event& e) override;
private:
	OpenEngine::OrthographicCameraController m_CameraController;
	
	OpenEngine::Ref<OpenEngine::VertexArray> m_SquareVA;
	OpenEngine::Ref<OpenEngine::Shader> m_Shader;

	OpenEngine::Ref<OpenEngine::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_Square2Color = { 0.8f, 0.2f, 0.3f, 1.0f };
};
