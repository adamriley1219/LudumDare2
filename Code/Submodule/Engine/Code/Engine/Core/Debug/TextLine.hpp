#pragma once
#include "Engine/Core/Graphics/Rgba.hpp"
#include <string>


struct TextLine
{
public:
	std::string m_text;
	Rgba		m_color;
	float		m_timeStamp;
	int			m_frameNumber;
public:
	explicit TextLine( std::string text, Rgba color, float timeStamp, int frameNumber );
	~TextLine(){}

};

