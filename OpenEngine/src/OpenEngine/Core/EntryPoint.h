#pragma once

#ifdef OE_PLATFORM_WINDOWS

extern OpenEngine::Application* OpenEngine::CreateApplication();

int main(int argc, char** argv)
{
	OpenEngine::Log::Init();
	OE_CORE_WARN("Initialzed Log!");

	auto app = OpenEngine::CreateApplication();
	app->Run();
	delete app;

}

#endif