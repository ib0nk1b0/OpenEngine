#include <OpenEngine.h>

#include "imgui/imgui.h"

class ExampleLayer : public OpenEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_TAB))
		{
			OE_TRACE("Tab key is being pressed (poll)!");
		}
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnEvent(OpenEngine::Event& event) override
	{
		if (event.GetEventType() == OpenEngine::EventType::KeyPressed)
		{
			OpenEngine::KeyPressedEvent& e = (OpenEngine::KeyPressedEvent&)event;
			if (e.GetKeyCode() == OE_KEY_TAB)
				OE_TRACE("Tab key is being pressed (event)!");
			OE_TRACE("{0}", (char)e.GetKeyCode());
		}

		if (event.GetEventType() == OpenEngine::EventType::MouseButtonPressed)
		{
			OpenEngine::MouseButtonPressedEvent& e = (OpenEngine::MouseButtonPressedEvent&)event;
			OE_TRACE("{0}", e.GetMouseButton());
		}
	}
};

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}