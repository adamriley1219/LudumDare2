#pragma once
#include "Engine/Core/Time/Clock.hpp"
#include <vector>


class StopWatch
{
public:
	StopWatch( Clock* clock );
	~StopWatch();

public:
	void SetAndReset( float time );  // I prefer the name "Start" for this - sets the duration and resets the timer from the start; 
	void Set( float time );          // Sets our duration, but maintains current elapsed time
	void Reset();                   // We have no elapsed time again
	void Stop();

	float GetDuration() const;       // how long is this timer for total
	float GetElapsedTime() const;    // How long as the timer been running;
	float GetRemainingTime() const;  // how much time until it ends
	float GetNormalizedElapsedTime() const; // 0 - 1 value saying how far I am into a duration (>1 if elapsed)

	bool IsStopped() const;
	bool HasElapsed() const;
	unsigned int GetElapseCount() const; 

	bool Decrement();          // remove one duration if we've elapsed (return whether it happened)
	unsigned int DecrementAll();       // return number of "durations" passed, and remove them; 

private:
	Clock* m_clock = nullptr; // Clock stopwatch is working off of (default to master)
	float m_startTime = 0.0f; 
	float m_duration = 0.0f; 

};

