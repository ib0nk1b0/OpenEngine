#pragma once

#ifdef OE_PLATFORM_WINDOWS

extern OpenEngine::Application* OpenEngine::CreateApplication();

int main(int argc, char** argv)
{
	OpenEngine::Log::Init();
	OE_CORE_WARN("Initialzed Log!");
	int a = 5;
	OE_INFO("Hello! Var={0}", a);


	auto app = OpenEngine::CreateApplication();
	app->Run();
	delete app;

}

#endif