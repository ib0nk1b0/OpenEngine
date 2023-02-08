#include <OpenEngine.h>

class ExampleLayer : public OpenEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Exampl")
	{
	}

	void OnUpdate() override
	{
		OE_INFO("ExampleLayer::Update");
	}

	void OnEvent(OpenEngine::Event& event) override
	{
		OE_TRACE("{0}", event);
	}
};

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new OpenEngine::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}