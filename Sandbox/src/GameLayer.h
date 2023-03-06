#pragma once

#include "OpenEngine.h"

#include "Paddle.h"
#include "Ball.h"

class GameLayer : public OpenEngine::Layer
{
public:
	GameLayer();
	virtual ~GameLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	bool CollisionRect(OpenEngine::Quad quad1, OpenEngine::Quad quad2);

	void OnUpdate(OpenEngine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(OpenEngine::Event& e) override;
private:
	OpenEngine::OrthographicCameraController m_CameraController;
	PaddleController m_PaddleController1, m_PaddleController2;
	OpenEngine::Ref<OpenEngine::Texture2D> m_CheckerboardTexture;
	Ball m_Ball;
};