#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace OpenEngine {

	class OpenEditor : public Application
	{
	public:
		OpenEditor()
			: Application("Open Editor")
		{
			PushLayer(new EditorLayer());
		}

		~OpenEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new OpenEditor();
	}

}