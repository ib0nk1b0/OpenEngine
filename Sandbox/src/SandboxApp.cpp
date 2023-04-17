#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>

#include "Sandbox2D.h"

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox(const OpenEngine::ApplicationSpecification& specification)
		: Application(specification)
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	OpenEngine::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../OpenEngine-Editor";

	return new Sandbox(spec);
}