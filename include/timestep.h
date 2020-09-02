#pragma once
#include <chrono>
#include <thread>

#include <SDL.h>

constexpr int DF_DESIRED_FPS = 60;
constexpr int DF_DESIRED_UPS = 30;

struct TimeStep
{
	//time unit is the microseconds !

	TimeStep()
		: m_iDesiredFramePS{ DF_DESIRED_FPS }
		, m_iDesiredUpdatePS{ DF_DESIRED_UPS }
		, m_aFrameTimes { }
		, m_dFrameTimeAccu{ 0 }
		,m_iFrameTimeIdx { 0 }
	{
		m_dStartTime = std::chrono::high_resolution_clock::now();
	}

	void startFrame()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		m_dLastFrameTime = newTime - m_dStartTime;
		m_dStartTime = newTime;

		//Avarage time
		m_dFrameTimeAccu += m_dLastFrameTime.count() - m_aFrameTimes[m_iFrameTimeIdx];
		m_aFrameTimes[m_iFrameTimeIdx] = m_dLastFrameTime.count();
		m_iFrameTimeIdx = (m_iFrameTimeIdx + 1) % 120;

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
	//For calculus in sec
	inline const double getDeltaTime()  const noexcept { return m_dPhyxTicks.count() / 1000.0 ; }
	//For Log
	inline const double timeSinceStart() const noexcept { return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - m_dStartTime).count(); }
	//FPS
	inline const double avgFps()  const noexcept { return 1000.0 / (m_dFrameTimeAccu /120); }
	//FrameTime in ms
	inline const double avgMsps()  const noexcept { return m_dFrameTimeAccu / 120; }

	//TODO : change granularity of timer resolution and use tick of high res clock (eg timeBeginPeriod/SDL_GetPerformanceFrequency   SDL_GetPerformanceCounter)
	void pause()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> sleepTime = m_dFrameTime - (newTime - m_dStartTime);

		if (sleepTime >= std::chrono::duration<double, std::milli>(1))
			SDL_Delay(sleepTime.count());//std::this_thread::sleep_for(sleepTime);

	}

	double m_aFrameTimes[120];

	std::chrono::high_resolution_clock::time_point m_dStartTime;

	std::chrono::duration<double, std::milli> m_dLastFrameTime = std::chrono::duration<double>(0);
	std::chrono::duration<double, std::milli> m_dPhyTimeAccum = std::chrono::duration<double>(0);

	int m_iDesiredFramePS;
	int m_iDesiredUpdatePS;

	const std::chrono::duration<double, std::milli> m_dPhyxTicks = std::chrono::duration<double>(1.0 / m_iDesiredUpdatePS);
	const std::chrono::duration<double, std::milli> m_dFrameTime = std::chrono::duration<double>(1.0 / m_iDesiredFramePS);

	double m_dFrameTimeAccu;

	int m_iFrameTimeIdx;

};
