#include <OpenEngine.h>

int num1 = 2;
 
class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}