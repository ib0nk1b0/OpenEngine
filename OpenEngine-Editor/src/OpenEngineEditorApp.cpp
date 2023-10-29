#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace OpenEngine {

	class OpenEditor : public Application
	{
	public:
		OpenEditor(const ApplicationSpecification& specificaiton)
			: Application(specificaiton)
		{
			PushLayer(new EditorLayer());
		}

		~OpenEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		ApplicationSpecification spec;
		spec.Name = "Open Editor";
		spec.CustomTitlebar = false;

		return new OpenEditor(spec);
	}

}