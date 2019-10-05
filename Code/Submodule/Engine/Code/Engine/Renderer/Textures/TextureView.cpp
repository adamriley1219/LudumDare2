#include "Engine/Renderer/Textures/TextureView.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Renderer/Textures/Sampler.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>

//--------------------------------------------------------------------------
/**
* Texture
*/
TextureView::TextureView()
{
}

//--------------------------------------------------------------------------
/**
* ~Texture
*/
TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_view);
	DX_SAFE_RELEASE(m_source);
}
