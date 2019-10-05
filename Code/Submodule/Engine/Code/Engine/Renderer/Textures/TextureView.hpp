#pragma once

class Sampler;
class RenderContext;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class TextureView 
{
	friend class Texture2D;
	friend class Material;
	friend class RenderContext;
public:
	TextureView();
	virtual ~TextureView();

	void SetSampler( Sampler *samp )       { m_sampler = samp; }

	ID3D11Resource* GetSorce() { return m_source; }

protected:
	// things this holds onto
	ID3D11ShaderResourceView *m_view = nullptr;  // D3D11 view we bind
	ID3D11Resource *m_source         = nullptr;  // resource this is a view of 

												 // things we're just referencing; 
	Sampler *m_sampler               = nullptr;  // optional sampler to use; 

												 // TODO: FAR FUTURE!
												 // this is a good use case for ref-counting (since each view should
												 // hold onto the texture it is a view of.  For now, we'll just
												 // taking advantage of D3D11's ref counting, but in other APIs (D3D12 or Vulkan)
												 // you would need to do this manually to keep the teture from disapearing from under the view
												 // std::shared_ptr<Texture> m_source_texture; 


};
