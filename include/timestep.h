#pragma once
#include <chrono>
#include <thread>

struct TimeStep
{
	//time unit is the milliseconds.

	TimeStep()
		: m_iDesiredFramePS{ 60 }
		, m_iDesiredUpdatePS{ 30 }
	{
		m_dStartTime = std::chrono::high_resolution_clock::now();
	}

	void startFrame()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		m_dLastFrameTime = newTime - m_dStartTime;
		m_dStartTime = newTime;

		//Prevent loop of death (if LastFrameTime> 3*m_dFrameTicks) (clamp the last frame time )
		if (m_dLastFrameTime > 3 * m_dFrameTime)
			m_dLastFrameTime = 3 * m_dFrameTime;

		m_dPhyTimeAccum += m_dLastFrameTime;
	}

	inline bool update()
	{
		if (m_dPhyTimeAccum >= m_dPhyxTicks)
		{
			m_dPhyTimeAccum -= m_dPhyxTicks;
			return true;
		}
		return false;
	}

	//For interpolation
	inline const double getAlpha() const noexcept { return m_dPhyTimeAccum / m_dPhyxTicks; }
	//For calculus
	inline const double getDeltaTime()  const noexcept { return m_dPhyxTicks.count() / 1000.0 ; }
	//For Log
	inline const double timeSinceStart() const noexcept { return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_dStartTime).count(); }
	//FPS
	inline const double fps()  const noexcept { return 1000.0 / m_dLastFrameTime.count(); } //NOTE : will be clamped if too high

	//TODO : change granularity of timer resolution (eg timeBeginPeriod)
	void pause()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> sleepTime = m_dFrameTime - (newTime - m_dStartTime);

		if (sleepTime >= std::chrono::duration<double, std::milli>(1))
			std::this_thread::sleep_for(sleepTime);
	}


	std::chrono::high_resolution_clock::time_point m_dStartTime;

	std::chrono::duration<double, std::milli> m_dLastFrameTime = std::chrono::duration<double>(0);
	std::chrono::duration<double, std::milli> m_dPhyTimeAccum = std::chrono::duration<double>(0);

	int m_iDesiredFramePS;
	int m_iDesiredUpdatePS;

	const std::chrono::duration<double, std::milli> m_dPhyxTicks = std::chrono::duration<double>(1.0 / m_iDesiredUpdatePS);
	const std::chrono::duration<double, std::milli> m_dFrameTime = std::chrono::duration<double>(1.0 / m_iDesiredFramePS);
};
