#pragma once

#include <memory>

#ifdef OE_PLATFORM_WINDOWS
	#ifdef OE_DYNAMIC_LINK
		#ifdef OE_BUILD_DLL
			#define OPENENGINE_API __declspec(dllexport)
		#else
			#define OPENENGINE_API __declspec(dllimport)
		#endif
	#else
		#define OPENENGINE_API
	#endif
#else
	#error OpenEngine only supports windows!
#endif

#ifdef OE_DEBUG
#define OE_ENABLE_ASSERTS
#endif

#ifdef OE_ENABLE_ASSERTS
#define OE_ASSERT(x, ...) { if(!(x)) { OE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define OE_CORE_ASSERT(x, ...) { if(!(x)) { OE_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define OE_ASSERT
	#define OE_CORE_ASSERT
#endif

#define BIT(x) (1 << x)

#define OE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace OpenEngine {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}