#pragma once

#ifdef OE_PLATFORM_WINDOWS

extern OpenEngine::Application* OpenEngine::CreateApplication();

int main(int argc, char** argv)
{
	OpenEngine::Log::Init();
	
	OE_PROFILE_BEGIN_SESSION("Startup", "OpenEngineProfile-Startup.json");
	auto app = OpenEngine::CreateApplication();
	OE_PROFILE_END_SESSION();

	OE_PROFILE_BEGIN_SESSION("Runtime", "OpenEngineProfile-Runtime.json");
	app->Run();
	OE_PROFILE_END_SESSION();

	OE_PROFILE_BEGIN_SESSION("Shutdown", "OpenEngineProfile-Shutdown.json");
	delete app;
	OE_PROFILE_END_SESSION();
}

#endif