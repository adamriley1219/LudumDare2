#include "Engine/Renderer/Textures/Texture2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Renderer/Textures/DepthStencilTargetView.hpp"
#include "Engine/Core/Graphics/Image.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>


//------------------------------------------------------------------------
static unsigned int DXBindFromUsage( unsigned int usage ) 
{
	unsigned int binds = 0U; 

	if (usage & TEXTURE_USAGE_TEXTURE_BIT) {
		binds |= D3D11_BIND_SHADER_RESOURCE; 
	}
	if (usage & TEXTURE_USAGE_COLOR_TARGET_BIT) {
		binds |= D3D11_BIND_RENDER_TARGET; 
	}
	if (usage & TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT) {
		binds |= D3D11_BIND_DEPTH_STENCIL; 
	}

	return binds; 
}




//--------------------------------------------------------------------------
/**
* Texture
*/
Texture2D::Texture2D( RenderContext* renderContext )
	: Texture( renderContext )
{
}

//--------------------------------------------------------------------------
/**
* ~Texture
*/
Texture2D::~Texture2D()
{

}


//--------------------------------------------------------------------------
/**
* CreateDepthStencilTarget
*/
Texture2D* Texture2D::CreateDepthStencilTarget( int width, int height )
{
	ID3D11Device *dd = m_owner->m_D3DDevice; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT | TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	MemZero( texDesc );

	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = DXUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);   // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		Texture2D* depthBuffer = new Texture2D( m_owner );
		depthBuffer->m_size = width * height; 
		depthBuffer->m_handle = tex2D;
		depthBuffer->m_dimensions = IntVec2( width, height );

		return depthBuffer; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "Shouldnt be here." ); // should be, just like to have the postcondition; 
		return nullptr; 
	}
}

//--------------------------------------------------------------------------
/**
* CreateDepthStencilTarget
*/
Texture2D* Texture2D::CreateDepthStencilTarget( RenderContext *ctx, int width, int height )
{
	ID3D11Device *dd = ctx->m_D3DDevice; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	eTextureUsageBits textureUsage = TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT | TEXTURE_USAGE_TEXTURE_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	eGPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	MemZero( texDesc );

	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = DXUsageFromMemoryUsage(memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(textureUsage);   // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		Texture2D* depthBuffer = new Texture2D( ctx );
		depthBuffer->m_size = width * height; 
		depthBuffer->m_handle = tex2D;
		depthBuffer->m_dimensions = IntVec2( width, height );
		return depthBuffer; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "Shouldnt be here." ); // should be, just like to have the postcondition; 
		return nullptr; 
	}
}

//--------------------------------------------------------------------------
/**
* CreateDepthStencilTargetView
*/
DepthStencilTargetView* Texture2D::CreateDepthStencilTargetView() const
{
	// if we don't have a handle, we can't create a view, so return nullptr
	ASSERT_RETURN_VALUE( m_handle != nullptr, "No handle to create DSV with.", nullptr );

	// get our device - since we're creating a resource
	ID3D11Device *dev = m_owner->m_D3DDevice; 
	ID3D11DepthStencilView *dsv = nullptr; 

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	MemZero( dsv_desc );
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	dev->CreateDepthStencilView( m_handle, &dsv_desc, &dsv );

	if (dsv != nullptr) {
		// Awesome, we have one
		DepthStencilTargetView *view = new DepthStencilTargetView();

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_dsv = dsv; 

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef(); 
		view->m_source = m_handle; 

		// copy the size over for convenience
		view->m_size = m_dimensions;

		// done - return!
		return view; 

	} else {
		return nullptr; 
	}
}

//--------------------------------------------------------------------------
/**
* CreateColorTargetView
*/
ColorTargetView* Texture2D::CreateColorTargetView() const
{
	ColorTargetView* ctv = new ColorTargetView();

	// Get a render target view of this
	m_owner->m_D3DDevice->CreateRenderTargetView( m_handle, nullptr, &ctv->m_rtv );

	D3D11_TEXTURE2D_DESC desc;
	( (ID3D11Texture2D*) m_handle )->GetDesc( &desc );
	ctv->m_size = IntVec2( desc.Width, desc.Height );

	return ctv;
}

//--------------------------------------------------------------------------
/**
* CreateDepthStencilTargetFor
*/
Texture2D* Texture2D::CreateDepthStencilTargetFor( Texture2D *colorTarget )
{
	return colorTarget->CreateDepthStencilTarget( colorTarget->m_dimensions.x, colorTarget->m_dimensions.y );
}

//--------------------------------------------------------------------------
/**
* CreateColorTarget
*/
Texture2D* Texture2D::CreateColorTarget( RenderContext* ctx, uint width, uint height )
{
	ID3D11Device* dd = ctx->m_D3DDevice; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	eTextureUsageBits textureUsage = TEXTURE_USAGE_COLOR_TARGET_BIT | TEXTURE_USAGE_TEXTURE_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	eGPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	MemZero( texDesc );

	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = DXUsageFromMemoryUsage(memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(textureUsage);   // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		Texture2D* buffer = new Texture2D( ctx );
		buffer->m_size = width * height; 
		buffer->m_handle = tex2D;
		buffer->m_dimensions = IntVec2( width, height );
		return buffer; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "Shouldnt be here." ); // should be, just like to have the postcondition; 
		return nullptr; 
	}
}

//--------------------------------------------------------------------------
/**
* CreateMatchingColorTarget
*/
Texture2D* Texture2D::CreateMatchingColorTarget( Texture2D *other )
{
	return CreateColorTarget( other->m_owner, other->m_dimensions.x, other->m_dimensions.y );
}

//--------------------------------------------------------------------------
/**
* LoadFromFile
*/
bool Texture2D::LoadFromFile( std::string const &filePath, bool flip )
{
	Image img( filePath.c_str(), flip );

	return LoadFromImage( img ); 
}

//--------------------------------------------------------------------------
/**
* LoadFromImage
*/
bool Texture2D::LoadFromImage( Image const &image )
{
	// cleanup old resources before creating new one just in case; 
	FreeHandles(); 

	ID3D11Device *dd = m_owner->m_D3DDevice;

	// If created from image, we'll assume it is only needed
	// as a read-only texture resource (if this is not true, change the
	// signature to take in the option)
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT;

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC texDesc;
	MemZero( texDesc );

	texDesc.Width = image.GetWidth();
	texDesc.Height = image.GetHeight();
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = DXUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);   // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Setup Initial Data
	// pitch is how many bytes is a single row of pixels;  
	unsigned int pitch = image.GetWidth() * image.GetBytesPerPixel(); // 4 bytes for an R8G8B8A8 format;  Just sub in four if your image is always 4 bytes per channel
	D3D11_SUBRESOURCE_DATA data;
	MemZero( data );
	data.pSysMem = image.GetBuffer();
	data.SysMemPitch = pitch;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		&data, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		m_size = image.GetSize(); 
		m_dimensions = image.GetDimensions();
		m_handle = tex2D;
		// TODO later assigment, generate mips if option is set; 

		return true; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "tex2D != nullptr in Texture2D::LoadFromImage but shouldn't get to this point." ); // should be, just like to have the postcondition; 
		return false; 
	}
}

//--------------------------------------------------------------------------
/**
* CreateTextureView2D
*/
TextureView2D* Texture2D::CreateTextureView2D() const
{
	// if we don't have a handle, we can't create a view, so return nullptr
	ASSERT_RETURN_VALUE( m_handle != nullptr, "Texture2D::CreateTextureView2D called without a handle in place.", nullptr );

	// 2D - we will want to eventually create specific views of a texture
	// and will want ot fill out a D3D11_SHADER_RESOURCE_VIEW_DESC, but for now
	// we'll just do the default thing (nullptr)

	// get our device - since we're creating a resource
	ID3D11Device* dev = m_owner->m_D3DDevice; 
	ID3D11ShaderResourceView* srv = nullptr; 
	dev->CreateShaderResourceView( m_handle, nullptr, &srv );

	if (srv != nullptr) {
		// Awesome, we have one
		TextureView2D* view = new TextureView2D(  );

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_view = srv; 

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef(); 
		view->m_source = m_handle; 

		// copy the size over for convenience
		view->m_dimensions = m_dimensions;

		// done - return!
		return view; 

	} else {
		return nullptr; 
	}
}

//--------------------------------------------------------------------------
/**
* FreeHandles
*/
void Texture2D::FreeHandles()
{
	DX_SAFE_RELEASE( m_handle );

}
