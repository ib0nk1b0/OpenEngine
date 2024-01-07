#pragma once

#include "Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

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

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
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