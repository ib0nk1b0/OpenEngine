#include <OpenEngine.h>
 
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