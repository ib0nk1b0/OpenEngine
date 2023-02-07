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

#ifdef OE_ENABLE_ASSERTS
	#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define OE_ASSERT (x, ...)
	#define OE_CORE_ASSERT (x, ...)
#endif

#define BIT(x) (1 << x)