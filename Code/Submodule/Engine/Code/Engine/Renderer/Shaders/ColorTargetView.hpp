#pragma once
#include "Engine/Math/IntVec2.hpp"

struct ID3D11RenderTargetView;

//--------------------------------------------------------------------------
class ColorTargetView
{
public:
	ColorTargetView() {}
	~ColorTargetView();

public:
	ID3D11RenderTargetView *m_rtv = nullptr; 
	IntVec2 m_size = IntVec2::ZERO;

};
