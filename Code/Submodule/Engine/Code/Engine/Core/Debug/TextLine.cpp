#include "Engine/Core/Debug/TextLine.hpp"





//--------------------------------------------------------------------------
/**
* TextLine
*/
TextLine::TextLine( std::string text, Rgba color, float timeStamp, int frameNumber )
	: m_text(text), m_color(color), m_timeStamp(timeStamp), m_frameNumber(frameNumber)
{
	
}
