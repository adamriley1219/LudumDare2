#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Math/MathUtils.hpp"



//--------------------------------------------------------------------------

Clock Clock::Master = Clock( nullptr );

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
* Clock
*/
Clock::Clock()
{
	SetParent( &Master );
}

//--------------------------------------------------------------------------
/**
* Clock
*/
Clock::Clock( Clock* parent )
{
	SetParent( parent );
}

//--------------------------------------------------------------------------
/**
* ~Clock
*/
Clock::~Clock()
{
	if ( m_parent != nullptr ) 
	{
		while ( m_children.size() > 0 ) 
		{
			m_children[0]->SetParent( m_parent ); 
		}
	}
}

//--------------------------------------------------------------------------
/**
* Step
*/
void Clock::Step( double deltaTime )
{
	deltaTime *= m_dilation; 
	if ( IsPaused() ) 
	{
		deltaTime = 0.0; 
	}

	deltaTime = Clamp( deltaTime + m_forcedTime, 0.0, m_frameLimit ); 

	++m_frameCount; 
	m_totalTime += deltaTime; 
	m_frameTime = deltaTime; 

	for( Clock *clockIter : m_children ) 
	{
		clockIter->Step( deltaTime ); 
	}

	m_forcedTime = 0.0;
}

//--------------------------------------------------------------------------
/**
* ForceStep
*/
void Clock::ForceStep( double deltaTime )
{
	m_forcedTime = deltaTime;
}

//--------------------------------------------------------------------------
/**
* SetFrameLimit
*/
void Clock::SetFrameLimit( double time )
{
	m_frameLimit = time;
}

//--------------------------------------------------------------------------
/**
* Dilate
*/
void Clock::Dilate( double dilation )
{
	m_dilation = dilation;
}

//--------------------------------------------------------------------------
/**
* Pause
*/
void Clock::Pause()
{
	++m_pauseCount;
}

//--------------------------------------------------------------------------
/**
* Resume
*/
void Clock::Resume()
{
	--m_pauseCount;
}

//--------------------------------------------------------------------------
/**
* ForcePause
*/
void Clock::ForcePause()
{
	m_pauseCount = 1;
}

//--------------------------------------------------------------------------
/**
* ForceResume
*/
void Clock::ForceResume()
{
	m_pauseCount = 0;
}

//--------------------------------------------------------------------------
/**
* SetParent
*/
void Clock::SetParent( Clock* newParent )
{
	if ( m_parent != nullptr )
	{
		m_parent->RemoveChild( this ); 
		m_parent = nullptr; 
	}

	m_parent = newParent; 
	if (m_parent != nullptr) 
	{
		m_parent->AddChild(this); 
	}}

//--------------------------------------------------------------------------
/**
* RemoveChild
*/
void Clock::RemoveChild( Clock* child )
{
	for( std::vector<Clock*>::iterator childItr = m_children.begin() ; childItr != m_children.end(); ++childItr )
	{
		if( (*childItr) == child )
		{
			m_children.erase( childItr );
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* AddChild
*/
void Clock::AddChild( Clock* child )
{
	m_children.push_back( child );
}

//--------------------------------------------------------------------------
/**
* IsPaused
*/
bool Clock::IsPaused()
{
	return m_pauseCount > 0;
}


//--------------------------------------------------------------------------
/**
* GetFrameTime
*/
double Clock::GetFrameTime() const
{
	return m_frameTime;
}

//--------------------------------------------------------------------------
/**
* GetFrameCount
*/
long unsigned int Clock::GetFrameCount() const
{
	return m_frameCount;
}

//--------------------------------------------------------------------------
/**
* GetTotalTime
*/
double Clock::GetTotalTime() const
{
	return m_totalTime;
}

//--------------------------------------------------------------------------
// System setup
//--------------------------------------------------------------------------

static double g_LastFrameTime = 0.0f;

//--------------------------------------------------------------------------
/**
* ClockSystemStartup
*/
void ClockSystemStartup()
{
	g_LastFrameTime = GetCurrentTimeSeconds();
	
}

//--------------------------------------------------------------------------
/**
* ClockSystemBeginFrame
*/
void ClockSystemBeginFrame()
{
	double cur_time = GetCurrentTimeSeconds();
	double delta_time = cur_time - g_LastFrameTime; 
	Clock::Master.Step( delta_time ); 
	g_LastFrameTime = cur_time;
}
