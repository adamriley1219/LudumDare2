#pragma once
#include "Engine/Renderer/RenderUtil.hpp"


class RenderContext;
struct ID3D11SamplerState;

class Sampler 
{
	friend class Texture2D;
public:
	Sampler();
	~Sampler();

	// called by the device when bound to lazily 
	// create the state object the first time, or if
	// it has changed (which is rare)
	void CreateStateIfDirty( RenderContext *ctx ); 

	inline void SetFilterModes( eFilterMode min, eFilterMode mag )    { m_minFilter = min; m_magFilter = mag; m_isDirty = true; }

	ID3D11SamplerState* GetHandle() const;

public:
	ID3D11SamplerState *m_handle  = nullptr; 

	// state we care about
	eFilterMode m_minFilter       = FILTER_MODE_POINT; // what to do when "shrinking" an image, or a pixel on screen represents multiple texels.
	eFilterMode m_magFilter       = FILTER_MODE_POINT; // what to do when "stretching" an image, or when multiple pixels on screen fit in the same texel; 
													   // eFilterMode m_mipFilter    = FILTER_MODE_POINT; // TODO when we implement mipmaps later; 

													   // there are more options, we'll introduce them as we need them
													   // ...wrap
													   // ...mip clamps & bias
													   // ...compare samplers

	bool m_isDirty                = true; // marked when state has changed so we know to recreate; 

};
