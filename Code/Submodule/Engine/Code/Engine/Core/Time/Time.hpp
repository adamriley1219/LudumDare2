//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <atomic>


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

uint64_t GetCurrentTimeHPC();
double HPCToSeconds( uint64_t hpc );