#include "Engine/Core/Debug/LogProfile.hpp"
#include "Engine/Core/Debug/Log.hpp"

//--------------------------------------------------------------------------
/**
* LogProfileScope
*/
LogProfileScope::LogProfileScope( const char* name )
{
	m_name = name;
	m_statTime = GetCurrentTimeHPC();
}

//--------------------------------------------------------------------------
/**
* ~LogProfileScope
*/
LogProfileScope::~LogProfileScope()
{
	uint64_t duration = GetCurrentTimeHPC() - m_statTime;
	Logf( "%s took %lf (%llu hpc)", m_name, HPCToSeconds(duration), duration ); 
}

