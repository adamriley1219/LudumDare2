#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Shaders/RenderBuffer.hpp"

struct ID3D11Resource;
class RenderContext;

enum eTextureUsageBit : unsigned int
{
	TEXTURE_USAGE_TEXTURE_BIT              = BIT_FLAG(0),    // Can be used to create a TextureView
	TEXTURE_USAGE_COLOR_TARGET_BIT         = BIT_FLAG(1),    // Can be used to create a ColorTargetView
	TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT = BIT_FLAG(2),    // Can be used to create a DepthStencilTargetView
}; 
typedef unsigned int eTextureUsageBits; 

class Texture
{
	friend class RenderContext;

public:
	Texture( RenderContext* renderContext );
	virtual ~Texture();

	unsigned int GetTextureID() const { return m_textureID; }

protected:
	unsigned int	m_textureID = 0;
	IntVec2			m_dimensions;
	unsigned int	m_size;
	RenderContext*	m_owner = nullptr;

protected:
	ID3D11Resource* m_handle = nullptr;
	eGPUMemoryUsage m_memoryUsage;
	eTextureUsageBits m_textureUsage;

};
