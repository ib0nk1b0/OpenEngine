#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>

#include "RuntimeLayer.h"

namespace OpenEngine {

	class Runtime : public OpenEngine::Application
	{
	public:
		Runtime(const OpenEngine::ApplicationSpecification& specification)
			: Application(specification)
		{
			PushLayer(new RuntimeLayer());
		}

		~Runtime()
		{
		}
	};

	OpenEngine::Application* OpenEngine::CreateApplication()
	{
		OpenEngine::ApplicationSpecification spec;
		spec.Name = "OpenEngine-Runtime";
		spec.WorkingDirectory = "../OpenEngine-Editor";
		spec.VSyncEnabled = false;

		return new Runtime(spec);
	}

}
