#pragma once
#include <vector>
#include <math.h>

class Clock
{
public:
	Clock();
	Clock( Clock* parent );
	~Clock();

	void Step( double deltaTime );
	void ForceStep( double deltaTime );

	void SetFrameLimit( double time ); 
	void Dilate( double dilation ); 

	void Pause(); 
	void Resume(); 
	void ForcePause();
	void ForceResume(); 

	void SetParent( Clock* newParent );
	void RemoveChild( Clock* child );
	void AddChild( Clock* child );

	bool IsPaused();

	double GetTotalTime() const;
	double GetFrameTime() const;
	long unsigned int GetFrameCount() const;

public:
	static Clock Master;

private:
	long unsigned int m_frameCount  = 0; 
	double m_totalTime				= 0.0; 
	double m_frameTime				= 0.0; 
	double m_forcedTime				= 0.0;

	double m_frameLimit				= INFINITY;  
	double m_dilation				= 1.0; 
	int m_pauseCount				= 0; 

	Clock *m_parent					= nullptr; 
	std::vector<Clock*> m_children; 

};

//------------------------------------------------------------------------
void ClockSystemStartup();

void ClockSystemBeginFrame();
