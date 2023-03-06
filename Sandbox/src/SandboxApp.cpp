#include <OpenEngine.h>
#include <OpenEngine/Core/EntryPoint.h>
//#include "Platform/OpenGL/OpenGLShader.h"
//#include "imgui/imgui.h"

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"
#include "GameLayer.h"

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new Sandbox2D());
		//PushLayer(new GameLayer());
	}

	~Sandbox()
	{
	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}