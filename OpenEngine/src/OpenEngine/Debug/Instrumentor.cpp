#include "oepch.h"
#include "Instrumentor.h"

#include "OpenEngine/Core/Application.h"

namespace OpenEngine {

	Timer::Timer(const std::string& name)
		: m_Name(name)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	Timer::~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Timer::Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;
		m_Time = duration * 0.001;

		Timing timing(m_Name, m_Time);
		OpenEngine::Application::Get().AddTiming(timing);
		m_Stopped = true;
	}

}