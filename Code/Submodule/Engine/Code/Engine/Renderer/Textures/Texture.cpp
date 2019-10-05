#include "Engine/Renderer/Textures/Texture.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>
//--------------------------------------------------------------------------
/**
* Texture
*/
Texture::Texture( RenderContext* renderContext )
{
	m_dimensions = IntVec2( 0, 0 );
	m_owner = renderContext;
}

//--------------------------------------------------------------------------
/**
* ~Texture
*/
Texture::~Texture()
{
	DX_SAFE_RELEASE( m_handle );

}
