#include "Engine/Renderer/Shaders/ColorTargetView.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include <d3d11.h>  
#include <DXGI.h>  
#include <dxgidebug.h>


//--------------------------------------------------------------------------
/**
* ~ColorTargetView
*/
ColorTargetView::~ColorTargetView()
{
	DX_SAFE_RELEASE(m_rtv);
}
