#pragma once

#ifdef OE_PLATFORM_WINDOWS
	#ifdef OE_BUILD_DLL
		#define OPENENGINE_API __declspec(dllexport)
	#else
		#define OPENENGINE_API __declspec(dllimport)
	#endif
#else
	#error OpenEngine only supports windows!
#endif