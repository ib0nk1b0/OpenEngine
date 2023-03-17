#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "GameLayer.h"

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{
		//PushLayer(new Sandbox2D());
		PushLayer(new GameLayer());
	}

	~Sandbox()
	{
	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}