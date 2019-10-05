#include "Engine/Renderer/Textures/DepthStencilTargetView.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <d3d11.h>  
#include <DXGI.h> 
#include <dxgidebug.h>

//--------------------------------------------------------------------------
/**
* DepthStencilTargetView
*/
DepthStencilTargetView::DepthStencilTargetView()
{

}

//--------------------------------------------------------------------------
/**
* ~DepthStencilTargetView
*/
DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE( m_source );
	DX_SAFE_RELEASE( m_dsv );
}

//--------------------------------------------------------------------------
/**
* GetWidth
*/
int DepthStencilTargetView::GetWidth() const
{
	return m_size.x;
}

//--------------------------------------------------------------------------
/**
* GetHeight
*/
int DepthStencilTargetView::GetHeight() const
{
	return m_size.y;
}

//--------------------------------------------------------------------------
/**
* ClearDepth
*/
void DepthStencilTargetView::ClearDepth( RenderContext* context, float depth )
{
	context->m_D3DContext->ClearDepthStencilView( m_dsv, D3D11_CLEAR_DEPTH, depth, 1 );
}

