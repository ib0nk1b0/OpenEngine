#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace OpenEngine {
	class OPENENGINE_API Log
	{
	public:
		static void Init();
		
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core Log macros
#define OE_CORE_TRACE(...)      ::OpenEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define OE_CORE_INFO(...)       ::OpenEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OE_CORE_WARN(...)       ::OpenEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OE_CORE_ERROR(...)      ::OpenEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OE_CORE_CRITICAL(...)   ::OpenEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log macros
#define OE_TRACE(...)           ::OpenEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define OE_INFO(...)            ::OpenEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define OE_WARN(...)            ::OpenEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OE_ERROR(...)           ::OpenEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define OE_CRITICAL(...)        ::OpenEngine::Log::GetClientLogger()->critical(__VA_ARGS__)