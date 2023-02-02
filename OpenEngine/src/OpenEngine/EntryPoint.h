#pragma once

#ifdef OE_PLATFORM_WINDOWS

extern OpenEngine::Application* OpenEngine::CreateApplication();

int main(int argc, char** argv)
{
	auto app = OpenEngine::CreateApplication();
	app->Run();
	delete app;

}

#endif