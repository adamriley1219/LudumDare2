#include "Engine/Core/Time/StopWatch.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------
/**
* StopWatch
*/
StopWatch::StopWatch( Clock* clock )
{
	m_clock = clock;
}

//--------------------------------------------------------------------------
/**
* ~StopWatch
*/
StopWatch::~StopWatch()
{

}

//--------------------------------------------------------------------------
/**
* SetAndReset
*/
void StopWatch::SetAndReset( float time )
{
	Set( time );
	Reset();
}

//--------------------------------------------------------------------------
/**
* Set
*/
void StopWatch::Set( float time )
{
	m_duration = time;
}

//--------------------------------------------------------------------------
/**
* Reset
*/
void StopWatch::Reset()
{
	m_startTime = (float) m_clock->GetTotalTime();
}

//--------------------------------------------------------------------------
/**
* Stop
*/
void StopWatch::Stop()
{
	m_duration = 0.0f;
}

//--------------------------------------------------------------------------
/**
* GetDuration
*/
float StopWatch::GetDuration() const
{
	return m_duration;
}

//--------------------------------------------------------------------------
/**
* GetElapsedTime
*/
float StopWatch::GetElapsedTime() const
{
	return m_duration <= 0.0f ? 0.0f : (float) m_clock->GetTotalTime() - m_startTime;
}

//--------------------------------------------------------------------------
/**
* GetRemainingTime
*/
float StopWatch::GetRemainingTime() const
{
	float res = m_duration - GetElapsedTime();
	return m_duration <= 0.0f ? 0.0f : res;
}

//--------------------------------------------------------------------------
/**
* GetNormalizedElapsedTime
*/
float StopWatch::GetNormalizedElapsedTime() const
{
	return m_duration <= 0.0f ? 0.0f : GetElapsedTime() / m_duration;
}

//--------------------------------------------------------------------------
/**
* IsStopped
*/
bool StopWatch::IsStopped() const
{
	return m_duration <= 0.0f;
}

//--------------------------------------------------------------------------
/**
* HasElapsed
*/
bool StopWatch::HasElapsed() const
{
	return m_duration <= 0.0f ? false : GetElapsedTime() >= m_duration;
}

//--------------------------------------------------------------------------
/**
* GetElapseCount
*/
unsigned int StopWatch::GetElapseCount() const
{
	return (int) GetNormalizedElapsedTime();
}

//--------------------------------------------------------------------------
/**
* Decrement
*/
bool StopWatch::Decrement()
{
	if( m_duration <= 0.0f )
	{
		return false;
	}
	if( HasElapsed() )
	{
		m_startTime += m_duration;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* DecrementAll
*/
unsigned int StopWatch::DecrementAll()
{
	if( m_duration <= 0.0f )
	{
		return false;
	}
	unsigned int count = GetElapseCount();
	m_startTime += m_duration * (float) count;
	return count;
}
