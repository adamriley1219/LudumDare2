//-----------------------------------------------------------------------------------------------
// Time.cpp
//	
#include "Engine/Core/Time/Time.hpp"
#include <chrono>


//-----------------------------------------------------------------------------------------------
double InitializeTime( uint64_t& out_initialTime )
{
	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	out_initialTime = now.time_since_epoch().count();

	std::chrono::seconds sec(1);
	return 1.0/static_cast<double>( std::chrono::nanoseconds(sec).count() );
}

static uint64_t TIME_initialTime;
static double TIME_secondsPerCount = InitializeTime( TIME_initialTime );

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	uint64_t currentCount = GetCurrentTimeHPC();
	uint64_t elapsedCountsSinceInitialTime = currentCount - TIME_initialTime;
	return HPCToSeconds( elapsedCountsSinceInitialTime );
}

//--------------------------------------------------------------------------
/**
* GetCurrentTimeHPC
*/
uint64_t GetCurrentTimeHPC()
{
	uint64_t hpc;
	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	hpc = now.time_since_epoch().count();
	return hpc;
}

//--------------------------------------------------------------------------
/**
* HPCToSeconds
*/
double HPCToSeconds( uint64_t hpc )
{
	double currentSeconds = static_cast< double >( hpc ) * TIME_secondsPerCount;	
	return currentSeconds;
}

