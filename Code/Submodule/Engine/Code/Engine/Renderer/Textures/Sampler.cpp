#include "Engine/Renderer/Textures/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <d3d11.h>  
#include <DXGI.h> 
#include <dxgidebug.h>

//--------------------------------------------------------------------------
/**
* Texture
*/
Sampler::Sampler()
{
}

//--------------------------------------------------------------------------
/**
* ~Texture
*/
Sampler::~Sampler()
{
	DX_SAFE_RELEASE( m_handle );
}

//--------------------------------------------------------------------------
/**
* CreateStateIfDirty
*/
void Sampler::CreateStateIfDirty( RenderContext *ctx )
{
	// no changes needed
	if (false == m_isDirty) {
		return; 
	}

	// Release old state; 
	DX_SAFE_RELEASE( m_handle ); 

	// Create new state
	D3D11_SAMPLER_DESC desc;
	MemZero( desc ); 

	desc.Filter = DXGetFilter( m_minFilter, m_magFilter );
	desc.MaxAnisotropy = 1U; // anistropic filtering (we're not using this... yet)

							 // set texture to wrap on UV, but clamp on W (mip).  
							 // (if you're smapling 3D wrapping noise, you may want wrap in all dimensions)
							 // (if you're doing single sprites, you may want clamp all so you don't get edge bleeding)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// set tha tcan can sample from all mip levels; 
	desc.MinLOD = -FLT_MAX; 
	desc.MaxLOD = FLT_MAX;   
	desc.MipLODBias = 0.0f;  // will shift the mip level for a sample by this

	desc.ComparisonFunc = D3D11_COMPARISON_NEVER; // will come into play doing shadow maps; 

												   // leaving border color black (only useful if our wrap mode is BORDER

												   // Create!
	ID3D11Device *d3dDevice = ctx->m_D3DDevice; 
	d3dDevice->CreateSamplerState( &desc, &m_handle ); 

	m_isDirty = false; 
	ASSERT_RECOVERABLE( m_handle != nullptr, "It filed but should never fail..." );   // this should honestly never fail.  If it does - check your output!

}

//--------------------------------------------------------------------------
/**
* GetHandle
*/
ID3D11SamplerState* Sampler::GetHandle() const
{
	return m_handle;
}
