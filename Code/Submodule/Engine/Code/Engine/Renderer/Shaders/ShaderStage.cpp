#include <d3d11.h>  
#include <DXGI.h>  
#include "Engine/Renderer/Shaders/ShaderStage.hpp"
#include "Engine/Renderer/RenderUtil.hpp"

//--------------------------------------------------------------------------
/**
* ~ShaderStage
*/
ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE( m_handle );
}
