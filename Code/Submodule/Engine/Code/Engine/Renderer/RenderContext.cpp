#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Core/Graphics/Image.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Textures/Texture.hpp"
#include "Engine/Renderer/Textures/Texture2D.hpp"
#include "Engine/Renderer/Textures/TextureView.hpp"
#include "Engine/Renderer/Textures/TextureView2D.hpp"
#include "Engine/Renderer/Textures/Sampler.hpp"
#include "Engine/Renderer/Textures/DepthStencilTargetView.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "ThirdParty/stb/stb_image.h"

#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Renderer/Shaders/UniformBuffer.hpp"
#include "Engine/Renderer/Shaders/VertexBuffer.hpp"
#include "Engine/Renderer/Shaders/IndexBuffer.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Model.hpp"

#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/XML/XMLUtils.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"

#include "Engine/Math/IntVec2.hpp"

#include <windows.h>
#include <string>

//------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------
#define WINDOW_TITLE    "D3D11 SETUP"


#define RENDER_DEBUG

#define WIN32_LEAN_AND_MEAN

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// D3D11 STUFF
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// D3D Stuff
// Required Headers
#include <d3d11.h>  
#include <DXGI.h>    

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code off line. 
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )


// R"()" is a "Raw String" 
const std::string DEFAULT_SHADER_TEXT = R"(
struct vs_input_t 
{
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
}; 

cbuffer world_constants : register(b1)
{
    float time; 
	float cosTime; 
	float worldWidth;
	float worldHeight;
};

cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION; 
};

cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

Texture2D<float4> tAlbedo : register(t0); // texutre I'm using for albedo (color) information
SamplerState sAlbedo : register(s0);      // sampler I'm using for the Albedo texture

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
}; 

float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return ( v - inMin ) * ( outMax - outMin ) / ( inMax - inMin ) + outMin; 
}

v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2f.position = clip_pos; 
   v2f.color = input.color; 
   v2f.uv = input.uv; 
    
   return v2f;
}

float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 

   // component wise multiply to "tint" the output
   float4 finalColor = texColor * input.color; 

   // output it; 
   return finalColor; 
}
)"; 

const std::string INVALID_SHADER_TEXT = R"(
struct vs_input_t 
{
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
}; 


cbuffer world_constants : register(b1)
{
    float time; 
	float cosTime; 
	float worldWidth;
	float worldHeight;
};

cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION; 
};

cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

Texture2D<float4> tAlbedo : register(t0); // texutre I'm using for albedo (color) information
SamplerState sAlbedo : register(s0);      // sampler I'm using for the Albedo texture

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
}; 

float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return ( v - inMin ) * ( outMax - outMin ) / ( inMax - inMin ) + outMin; 
}

v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2f.position = clip_pos; 
   v2f.color = float4( 1.0f, 0.0f, 1.0f, 1.0f ); 
   v2f.uv = input.uv; 
    
   return v2f;
}

float hex(float u, float v) 
{
    u *= 1.1547;
    v += fmod( floor(u), 2.0f ) * 0.5f;
    u = abs( (fmod( u, 1.f ) - 0.5f ) );
    v = abs( (fmod( v, 1.f ) - 0.5f ) );
    return abs( max( u * 1.5f + v, v * 2.0f ) - 1.0f ) ;
}

float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	float u = input.position.x / worldWidth;
	float v = input.position.y / worldHeight;

    u *= worldWidth / worldHeight;
    
    float h = hex(u * 18.0f, v * 18.0f);
    
    float d = max(.1f - abs( sin( length( float2( u, v ) * 11.0f ) + time ) * 0.5 + 0.5 - h) , 0.f);
    return float4( .9f, .2f, .8f, 1.f ) + d * 2.0f ;

}
)"; 



const std::string DEFAULT_UNLIT_SHADER_TEXT = R"(
struct vs_input_t 
{
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 
}; 

cbuffer camera_constants : register(b2)
{
   float4x4 VIEW; 
   float4x4 PROJECTION; 
};

cbuffer model_constants : register(b3)
{
   float4x4 MODEL;  // LOCAL_TO_WORLD
}

Texture2D<float4> tAlbedo : register(t0); // texutre I'm using for albedo (color) information
SamplerState sAlbedo : register(s0);      // sampler I'm using for the Albedo texture

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
}; 

float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return ( v - inMin ) * ( outMax - outMin ) / ( inMax - inMin ) + outMin; 
}

v2f_t VertexFunction(vs_input_t input)
{
   v2f_t v2f = (v2f_t)0;

   float4 local_pos = float4( input.position, 1.0f ); 
   float4 world_pos = mul( MODEL, local_pos );
   float4 view_pos = mul( VIEW, world_pos ); 
   float4 clip_pos = mul( PROJECTION, view_pos ); 

   v2f.position = clip_pos; 
   v2f.color = input.color; 
   v2f.uv = input.uv; 
    
   return v2f;
}

float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // First, we sample from our texture
   float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 

   // component wise multiply to "tint" the output
   float4 finalColor = texColor * input.color; 

   // output it; 
   return finalColor; 
}
)"; 

//--------------------------------------------------------------------------
/**
* RenderContext
*/
RenderContext::RenderContext( WindowContext* displayHandle )
{
	m_windowContext = displayHandle;
	HWND handle = (HWND) displayHandle->m_hwnd;

	uint device_flags = 0U;
#if defined(RENDER_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

	// Setup our Swap Chain

	DXGI_SWAP_CHAIN_DESC swapDesc;
	memset( &swapDesc, 0, sizeof(swapDesc) );

	// fill the swap chain description struct
	swapDesc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;                                  
	swapDesc.OutputWindow = handle;                                // the window to be copied to on present
	swapDesc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

																 // Default options.
	swapDesc.Windowed = TRUE;                                    // windowed/full-screen mode
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	m_clientDims = displayHandle->GetClientDims();
	swapDesc.BufferDesc.Width = m_clientDims.x;
	swapDesc.BufferDesc.Height = m_clientDims.y;
	

	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain( nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
		nullptr,                   // Software Module - DLL that implements software mode (we do not use)
		device_flags,              // device creation options
		nullptr,                   // feature level (use default)
		0U,                        // number of feature levels to attempt
		D3D11_SDK_VERSION,         // SDK Version to use
		&swapDesc,                // Description of our swap chain
		&m_D3DSwapChain,            // Swap Chain we're creating
		&m_D3DDevice,               // [out] The device created
		nullptr,                   // [out] Feature Level Acquired
		&m_D3DContext );            // Context that can issue commands on this pipe.

									// SUCCEEDED & FAILED are macros provided by Windows to checking
									// the results.  Almost every D3D call will return one - be sure to check it.
	GUARANTEE_OR_DIE( SUCCEEDED(hr), "The ContextRenderer was not sucessful in setup." );

}

//--------------------------------------------------------------------------
/**
* ~RenderContext
*/
RenderContext::~RenderContext()
{

}

//--------------------------------------------------------------------------
/**
* Startup
*/
void RenderContext::Startup()
{
	// Tells the context that we are using 3 point triangles.
	m_D3DContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	Sampler *point = new Sampler(); 
	point->SetFilterModes( FILTER_MODE_LINEAR, FILTER_MODE_POINT );  // still min linear - to get blending as it gets smaller; 
	m_samplers[SAMPLE_MODE_POINT] = point; 

	Sampler *linear = new Sampler(); 
	linear->SetFilterModes( FILTER_MODE_LINEAR, FILTER_MODE_LINEAR ); 
	m_samplers[SAMPLE_MODE_LINEAR] = linear; 

	// premake defaults 
	//  ------ May not work -------
	Image image;	
	Texture2D* white = new Texture2D( this );
	white->LoadFromImage(image);
	m_loadedTextureViews["white"] = white->CreateTextureView2D();
	delete white;

	image.SetTexelColor( 0, 0, Rgba( 0.5f, 0.5f, 1.0f, 1.0f ) );
	image.SetFilepath( "flat" );
	Texture2D* flat = new Texture2D( this );
	flat->LoadFromImage(image);
	m_loadedTextureViews["flat"] = flat->CreateTextureView2D();
	delete flat;

	image.SetTexelColor( 0, 0, Rgba( 0.0f, 0.0f, 0.0f, 1.0f ) );
	image.SetFilepath( "black" );
	Texture2D* black = new Texture2D( this );
	black->LoadFromImage(image);
	m_loadedTextureViews["black"] = black->CreateTextureView2D();
	delete black;

	BindTextureView( TEXTURE_SLOT_ALBEDO, "white" );
	BindTextureView( TEXTURE_SLOT_NORMAL, "flat" );
	BindTextureView( TEXTURE_SLOT_EMISSIVE, "black" );

	m_shader = CreateOrGetShaderFromSource( "DEFAULT", DEFAULT_SHADER_TEXT );
	CreateOrGetShaderFromSource( "INVALID", INVALID_SHADER_TEXT );
	CreateOrGetShaderFromSource( "DEFAULT_UNLIT", DEFAULT_UNLIT_SHADER_TEXT );
	BindShader( m_shader );

	BindModelMatrix( Matrix44::IDENTITY ); 

	m_immediateVBO = new VertexBuffer( this );
	m_immediateVBO->m_layout = Vertex_PCU::LAYOUT;
	m_immediateVBO->m_stride = sizeof(Vertex_PCU);

}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void RenderContext::BeginFrame()
{
	// Get Back Buffer
	ID3D11Texture2D* backBuffer = nullptr;
	m_D3DSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )& backBuffer );
	
	// Get a render target view of this
	ID3D11RenderTargetView* rtv;
	m_D3DDevice->CreateRenderTargetView( backBuffer, nullptr, &rtv );

	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc( &desc );
	IntVec2 buffsize(desc.Width, desc.Height );

	m_ctvTexture = Texture2D::CreateColorTarget( this, buffsize.x, buffsize.y );
	m_colorTargetView = m_ctvTexture->CreateColorTargetView();
	m_ctvTextureView = m_ctvTexture->CreateTextureView2D();

	Texture2D* defaultDepthTexture = Texture2D::CreateDepthStencilTarget( this, m_colorTargetView->m_size.x, m_colorTargetView->m_size.y );
	m_defaultDSV = defaultDepthTexture->CreateDepthStencilTargetView();
	m_defaultDSV->ClearDepth( this, 1.0f );


	if( !m_scratchBuffer )
	{
		m_scratchTexture = Texture2D::CreateMatchingColorTarget( m_ctvTexture );
		m_scratchBuffer = m_scratchTexture->CreateColorTargetView();
	}

	SAFE_DELETE( defaultDepthTexture );
	DX_SAFE_RELEASE( backBuffer );
	DX_SAFE_RELEASE( rtv );

}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void RenderContext::EndFrame()
{
	ID3D11Texture2D* backBuffer = nullptr;
	m_D3DSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )& backBuffer );

	Texture2D texture( this );
	texture.m_handle = backBuffer;
	CopyTexture( &texture, m_ctvTexture );

	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	m_D3DSwapChain->Present( 0, 0 );
	SAFE_DELETE( m_defaultDSV );
	SAFE_DELETE( m_ctvTexture );
	SAFE_DELETE( m_colorTargetView );
	SAFE_DELETE( m_ctvTextureView );

}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void RenderContext::Shutdown()
{
	for( auto p : m_loadedShaders )
	{
		if( p.second )
		{
			delete p.second;
			p.second = nullptr;
		}
	}
	m_loadedShaders.clear();

	for( auto p : m_loadedFonts )
	{
		delete p.second;
		p.second = nullptr;
	}
	m_loadedFonts.clear();

	for( auto p : m_loadedTextureViews )
	{
		delete p.second;
		p.second = nullptr;
	}
	m_loadedTextureViews.clear();

	for( auto p : m_loadedMaterials )
	{
		delete p.second;
		p.second = nullptr;
	}
	m_loadedMaterials.clear();

	for( auto p : m_loadedMeshes )
	{
		delete p.second;
		p.second = nullptr;
	}
	m_loadedMeshes.clear();

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	SAFE_DELETE( m_modelUBO );
	SAFE_DELETE( m_lightUBO );
	SAFE_DELETE( m_scratchBuffer );
	SAFE_DELETE( m_scratchTexture );

	delete m_samplers[SAMPLE_MODE_POINT]; 
	m_samplers[SAMPLE_MODE_POINT] = nullptr;
	delete m_samplers[SAMPLE_MODE_LINEAR]; 
	m_samplers[SAMPLE_MODE_LINEAR] = nullptr;

// 	if( m_D3DDevice == nullptr ) {
// 		return;
// 	}
// 	ID3D11Debug* debug = nullptr;
// 	HRESULT result = m_D3DDevice->QueryInterface( __uuidof(ID3D11Debug), (void**)&debug );
// 	if( SUCCEEDED(result) ) {
// 		debug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
// 	}
// 	DX_SAFE_RELEASE( debug );


	DX_SAFE_RELEASE(m_D3DDevice);
	DX_SAFE_RELEASE(m_D3DContext);
	DX_SAFE_RELEASE(m_D3DSwapChain);
}

//--------------------------------------------------------------------------
/**
* CreateTextureViewFromFile
*/
TextureView2D* RenderContext::CreateTextureViewFromFile( const char* imageFilePath, bool flip )
{
	TextureView2D *view = nullptr; 

	// normal stuff - if it exists, return it; 
	auto item = m_loadedTextureViews.find( imageFilePath ); 
	if (item != m_loadedTextureViews.end()) 
	{
		return item->second; 
	} 

	Texture2D* tex = new Texture2D(this); 
	if (!tex->LoadFromFile( imageFilePath, flip ) ) 
	{
		delete tex;
		return nullptr;
	} 
	else 
	{
		// create the view
		view = tex->CreateTextureView2D(); 

		delete tex;  

		m_loadedTextureViews[imageFilePath] = view; 
	}
	return view; 
}

//--------------------------------------------------------------------------
/**
* CreateOrGetTextureViewFromFile
*/
TextureView2D* RenderContext::CreateOrGetTextureViewFromFile( const char* imageFilePath, bool flip )
{
	std::map< std::string,  TextureView2D* >::iterator itr = m_loadedTextureViews.find( imageFilePath );
	if( itr != m_loadedTextureViews.end() )
	{
		TextureView2D* textureView = itr->second;
		return textureView;
	}
	else
	{
		TextureView2D* textureView = CreateTextureViewFromFile( imageFilePath, flip );
		m_loadedTextureViews[imageFilePath] = textureView;
		return textureView;
	}
}

//--------------------------------------------------------------------------
/**
* CreateOrGetBitmapFromFile
*/
BitmapFont* RenderContext::CreateOrGetBitmapFromFile( const char* fontName )
{
	std::map< std::string, BitmapFont* >::iterator itr = m_loadedFonts.find( fontName );
	if( itr != m_loadedFonts.end())
	{
		BitmapFont* bitmap = itr->second;
		return bitmap;
	}
	else
	{
		BitmapFont* bitMap = CreateBitmapFromFile( fontName );
		bitMap->GetTextureView()->SetSampler( m_samplers[SAMPLE_MODE_POINT] );
		m_loadedFonts[fontName] = bitMap;
		return bitMap;
	}
}

//--------------------------------------------------------------------------
/**
* CreateOrGetMesh
*/
MeshGPU* RenderContext::CreateOrGetMesh( const char* meshName )
{
	tinyxml2::XMLDocument config;
	config.LoadFile( Stringf( "Data/Models/%s.mesh", meshName ).c_str() );
	XmlElement* root = config.RootElement();
	return CreateOrGetMesh( root );
}

//--------------------------------------------------------------------------
/**
* CreateOrGetMesh
*/
MeshGPU* RenderContext::CreateOrGetMesh( XmlElement* root )
{
	if( root )
	{
		std::string src = ParseXmlAttribute( *root, "src", "ERROR_NO_ID" );
		std::map< std::string, MeshGPU* >::iterator itr = m_loadedMeshes.find( src );
		if( itr != m_loadedMeshes.end())
		{
			MeshGPU* mesh = itr->second;
			return mesh;
		}
		else
		{
			MeshCPU* mesh = CreateMesh( root );
			MeshGPU* gpuMesh = new MeshGPU( this );

			if( mesh->ContainsTangents() )
			{
				gpuMesh->CopyFromCPUMesh<Vertex_LIT>( mesh );
			}
			else
			{
				gpuMesh->CopyFromCPUMesh<Vertex_PCU>( mesh );
			}
			m_loadedMeshes[src] = gpuMesh;
			delete mesh;
			return gpuMesh;
		}
	}
	ERROR_RECOVERABLE( "Root not valid in RenderContext::CreateOrGetMesh" );
	return nullptr;
}

//--------------------------------------------------------------------------
/**
* CreateOrGetMeshWithSubmeshes
*/
std::vector<MeshGPU*> RenderContext::CreateOrGetMeshWithSubmeshes( const char* meshName )
{
	std::vector<MeshGPU*> meshes;

	tinyxml2::XMLDocument config;
	config.LoadFile( Stringf( "Data/Models/%s.mesh", meshName ).c_str() );
	XmlElement* root = config.RootElement();

	if( root )
	{
		XmlElement* meshEle = root->FirstChildElement( "submesh" );
		if( meshEle )
		{
			for( meshEle; meshEle != nullptr; meshEle = meshEle->NextSiblingElement( "submesh" ) )
			{
				meshes.push_back( CreateOrGetMesh( meshEle ) );
			}
		}
		else
		{
			meshes.push_back( CreateOrGetMesh( root ) );
		}
	}
	ASSERT_RECOVERABLE( root, "root not valid in RenderContext::CreateOrGetMeshWithSubmeshes" );
	return meshes;
}

//--------------------------------------------------------------------------
/**
* CreateOrGetMaterialFromXML
*/
Material* RenderContext::CreateOrGetMaterialFromXML( const char* matPath, bool flipTextures /*= false*/ )
{
	std::map< std::string, Material* >::iterator itr = m_loadedMaterials.find( matPath );
	if( itr != m_loadedMaterials.end())
	{
		Material* material = itr->second;
		return material;
	}
	else
	{
		Material* material = CreateMaterialFromXML( matPath, flipTextures );
		m_loadedMaterials[matPath] = material;
		return material;
	}
}

//--------------------------------------------------------------------------
/**
* CreateBitmapFromFile
*/
BitmapFont* RenderContext::CreateBitmapFromFile( const char* fontName )
{
	TextureView* texture = CreateOrGetTextureViewFromFile( Stringf( "Data/Fonts/%s.png", fontName ).c_str(), true );
	return new BitmapFont( fontName, texture );
}

//--------------------------------------------------------------------------
/**
* CreateMeshFromFile
*/
MeshCPU* RenderContext::CreateMesh( XmlElement* root )
{
	if( root )
	{
		MeshCPU* mesh  = new MeshCPU();

		std::string src = ParseXmlAttribute( *root, "src", "ERROR_NO_SRC" );
		src = Stringf( "Data/Models/%s", src.c_str() );
		std::string id = ParseXmlAttribute( *root, "id", "UNKNOWN_ID" );
		bool invert = ParseXmlAttribute( *root, "invert", false );
		bool tangents = ParseXmlAttribute( *root, "tangents", false );
		std::string scaleAsString = ParseXmlAttribute( *root, "scale", "1.0f" );
		std::string transform = ParseXmlAttribute( *root, "transform", "x y z" );
		mesh->m_defaultMaterial = ParseXmlAttribute( *root, "material", mesh->m_defaultMaterial );
		float scale = EquateString( scaleAsString );
		mesh->LoadFromObjFile( src, invert, tangents, scale, transform );

		ASSERT_RECOVERABLE( src != "ERROR_NO_SRC", Stringf( "Error: Didnt find a src within file: %s", id.c_str() ).c_str() );
		return mesh;
	}
	return new MeshCPU();
}

//--------------------------------------------------------------------------
/**
* CreateMaterialFromXML
*/
Material* RenderContext::CreateMaterialFromXML( const char* filePath, bool flipTextures /*= false*/ )
{
	tinyxml2::XMLDocument config;
	config.LoadFile(filePath);
	XmlElement* root = config.RootElement();

	if( root )
	{
		return new Material( this, root, flipTextures );
	}
	return new Material( this );
}

//--------------------------------------------------------------------------
/**
* CreateShaderFromFile
*/
Shader* RenderContext::CreateShaderFromFile( const char* filename )
{
	char* data = nullptr;
	unsigned long int sizeFile = LoadFile( filename, data ); 
	if( sizeFile <= 0 )
	{
		return new Shader( m_loadedShaders["INVALID"] );
	}

	// Compile hlsl to byte code (generic compiled version)
	ID3DBlob *vs_bytecode = CompileHLSLToShaderBlob( filename, data, sizeFile, "VertexFunction", "vs_5_0" );
	ID3DBlob *fs_bytecode = CompileHLSLToShaderBlob( filename, data, sizeFile, "FragmentFunction", "ps_5_0" );

	if( !vs_bytecode || !vs_bytecode)
	{
		return new Shader( m_loadedShaders["INVALID"] );
	}

	Shader* newShader = new Shader( filename, this );

	m_D3DDevice->CreateVertexShader( vs_bytecode->GetBufferPointer(), 
		vs_bytecode->GetBufferSize(), 
		nullptr, 
		&newShader->m_vertexShaderStage.m_vertexShader );

	m_D3DDevice->CreatePixelShader( fs_bytecode->GetBufferPointer(), 
		fs_bytecode->GetBufferSize(), 
		nullptr, 
		&newShader->m_fragmentShaderStage.m_pixelShader );

	newShader->m_VSbytecode = vs_bytecode;
	DX_SAFE_RELEASE( fs_bytecode );
	delete data;
	return newShader;
}

//--------------------------------------------------------------------------
/**
* CreateShaderFromXML
*/
Shader* RenderContext::CreateShaderFromXML( const char* filename )
{
	tinyxml2::XMLDocument config;
	config.LoadFile(filename);
	XmlElement* root = config.RootElement();
	
	if( root )
	{
	
	XmlElement* shaderInfoEle = root->FirstChildElement();
	
		if( shaderInfoEle )
		{
			XmlElement* vertEle	 = shaderInfoEle->FirstChildElement( "vert" );
			XmlElement* fragEle	 = shaderInfoEle->FirstChildElement( "frag" );
			XmlElement* depthEle = shaderInfoEle->FirstChildElement( "depth" );
			XmlElement* blendEle = shaderInfoEle->FirstChildElement( "blend" );	
			XmlElement* rasterEle = shaderInfoEle->FirstChildElement( "raster" );	

			std::string filepath = ParseXmlAttribute( *shaderInfoEle, "src", "" );
			std::string vertFunc = ParseXmlAttribute( *vertEle, "entry", "VertexFunction" );
			std::string fragFunc = ParseXmlAttribute( *fragEle, "entry", "FragmentFunction" );

			bool depthWrite = ParseXmlAttribute( *depthEle, "write", true );
			std::string depthTestType = ParseXmlAttribute( *depthEle, "test", "" );
			std::string blendMode = ParseXmlAttribute( *blendEle, "mode", "" );

			bool wireframe = ParseXmlAttribute( *rasterEle, "wireframe", false );
			bool clockwise = ParseXmlAttribute( *rasterEle, "clockwise", false );
			std::string cull = ParseXmlAttribute( *rasterEle, "cull", "back" );

			char* data = nullptr;
			unsigned long int sizeFile = LoadFile( filepath.c_str(), data ); 
			if( sizeFile <= 0 )
			{
				return new Shader( m_loadedShaders["INVALID"] );
			}

			// Compile hlsl to byte code (generic compiled version)
			ID3DBlob *vs_bytecode = CompileHLSLToShaderBlob( filename, data, sizeFile, vertFunc.c_str(), "vs_5_0" );
			ID3DBlob *fs_bytecode = CompileHLSLToShaderBlob( filename, data, sizeFile, fragFunc.c_str(), "ps_5_0" );

			if( !vs_bytecode || !vs_bytecode)
			{
				return new Shader( m_loadedShaders["INVALID"] );
			}


			Shader* newShader = new Shader( filename, this );

			m_D3DDevice->CreateVertexShader( vs_bytecode->GetBufferPointer(), 
				vs_bytecode->GetBufferSize(), 
				nullptr, 
				&newShader->m_vertexShaderStage.m_vertexShader );

			m_D3DDevice->CreatePixelShader( fs_bytecode->GetBufferPointer(), 
				fs_bytecode->GetBufferSize(), 
				nullptr, 
				&newShader->m_fragmentShaderStage.m_pixelShader );

			newShader->m_VSbytecode = vs_bytecode;
			DX_SAFE_RELEASE( fs_bytecode );
			delete data;

			newShader->SetBlendMode( blendMode );
			newShader->SetDepthState( depthTestType, depthWrite );
			newShader->SetRasterState( cull, wireframe, clockwise );

			return newShader;
		}
	}
	return new Shader( m_loadedShaders["INVALID"] );
}

//--------------------------------------------------------------------------
/**
* CreateShaderFromSource
*/
Shader* RenderContext::CreateShaderFromSource( const char* filename, std::string source )
{
	// Compile hlsl to byte code (generic compiled version)
	ID3DBlob *vs_bytecode = CompileHLSLToShaderBlob( filename, source.c_str(), (unsigned long int)source.size(), "VertexFunction", "vs_5_0" );
	ID3DBlob *fs_bytecode = CompileHLSLToShaderBlob( filename, source.c_str(), (unsigned long int)source.size(), "FragmentFunction", "ps_5_0" );

	if( !vs_bytecode || !vs_bytecode)
	{
		return new Shader( m_loadedShaders["INVALID"] );
	}

	Shader* newShader = new Shader( filename, this );

	m_D3DDevice->CreateVertexShader( vs_bytecode->GetBufferPointer(), 
		vs_bytecode->GetBufferSize(), 
		nullptr, 
		&newShader->m_vertexShaderStage.m_vertexShader );

	m_D3DDevice->CreatePixelShader( fs_bytecode->GetBufferPointer(), 
		fs_bytecode->GetBufferSize(), 
		nullptr, 
		&newShader->m_fragmentShaderStage.m_pixelShader );

	newShader->m_VSbytecode = vs_bytecode;
	DX_SAFE_RELEASE( fs_bytecode );
	return newShader;
}

//--------------------------------------------------------------------------
/**
* CreateOrGetShaderFromFile
*/
Shader* RenderContext::CreateOrGetShaderFromFile( const char* filename )
{
	std::map< std::string,  Shader* >::iterator itr = m_loadedShaders.find( filename );
	if( itr != m_loadedShaders.end() )
	{
		Shader* shader = itr->second;
		return shader;
	}
	else
	{
		Shader* shader = CreateShaderFromFile( filename );
		m_loadedShaders[filename] = shader;
		return shader;
	}
}

//--------------------------------------------------------------------------
/**
* CreateOrGetShaderFromXML
*/
Shader* RenderContext::CreateOrGetShaderFromXML( const char* filename )
{
	std::map< std::string,  Shader* >::iterator itr = m_loadedShaders.find( filename );
	if( itr != m_loadedShaders.end() )
	{
		Shader* shader = itr->second;
		return shader;
	}
	else
	{
		Shader* shader = CreateShaderFromXML( filename );
		m_loadedShaders[filename] = shader;
		return shader;
	}
}

//--------------------------------------------------------------------------
/**
* CreateOrGetShaderFromSorce
*/
Shader* RenderContext::CreateOrGetShaderFromSource( const char* filename, std::string source )
{
	std::map< std::string,  Shader* >::iterator itr = m_loadedShaders.find( filename );
	if( itr != m_loadedShaders.end() )
	{
		Shader* shader = itr->second;
		return shader;
	}
	else
	{
		Shader* shader = CreateShaderFromSource( filename, source );
		m_loadedShaders[filename] = shader;
		return shader;
	}
}

//--------------------------------------------------------------------------
/**
* RegisterMesh
*/
MeshGPU* RenderContext::RegisterMesh( MeshGPU* mesh, const std::string& name )
{
	if( m_loadedMeshes.find(name) != m_loadedMeshes.end() )
	{
		delete m_loadedMeshes[name];
	}
	m_loadedMeshes[name] = mesh;
	return mesh;
}

//--------------------------------------------------------------------------
/**
* RegisterMesh
*/
MeshGPU* RenderContext::RegisterMesh( MeshCPU* mesh, const std::string& name )
{
	if( m_loadedMeshes.find(name) != m_loadedMeshes.end() )
	{
		delete m_loadedMeshes[name];
	}

	MeshGPU* gpuMesh = new MeshGPU( this );
	if( mesh->ContainsTangents() )
	{
		gpuMesh->CopyFromCPUMesh<Vertex_LIT>( mesh );
	}
	else
	{
		gpuMesh->CopyFromCPUMesh<Vertex_PCU>( mesh );
	}

	m_loadedMeshes[name] = gpuMesh;
	return gpuMesh;
}

//--------------------------------------------------------------------------
/**
* RegisterTextureView
*/
TextureView2D* RenderContext::RegisterTextureView( Texture2D* texture, const std::string& path )
{
	TextureView2D* view = nullptr;
	if( texture )
	{
		// create the view
		if( m_loadedTextureViews.find( path ) == m_loadedTextureViews.end() )
		{
			view = texture->CreateTextureView2D(); 
			m_loadedTextureViews[path] = view;
		}
	}
	return view;
}

//--------------------------------------------------------------------------
/**
* CreateTextureFromFileBindTexture
*/
void RenderContext::BindTextureView( unsigned int slot, const TextureView* view )
{
	ID3D11ShaderResourceView *srv = nullptr; 
	if (view != nullptr) {
		srv = view->m_view; 
	} else {
		//srv = GetOrCreateTextureView2D("white");
		switch( slot )
		{
		case TEXTURE_SLOT_ALBEDO:
			srv = m_loadedTextureViews["white"]->m_view;
			break;
		case TEXTURE_SLOT_NORMAL:
			srv = m_loadedTextureViews["flat"]->m_view;
			break;
		case TEXTURE_SLOT_EMISSIVE:
			srv = m_loadedTextureViews["black"]->m_view;
			break;
		case TEXTURE_SLOT_SPECULAR:
			srv = m_loadedTextureViews["white"]->m_view;
			break;
		default:
			srv = m_loadedTextureViews["white"]->m_view;
			break;
		}
		// TODO - if view is nullptr, default to something
		// that makes since for the slot
		// For now - bind a solid "WHITE" m_ctvTexture if slot == 0; 
	}

	// You can bind textures to the Vertex stage, but not samplers
	// We're *not* for now since no effect we do at Guildhall requires it, but
	// be aware it is an option; 
	// m_context->VSSetShaderResource( slot, 1U, &srv ); 

	m_D3DContext->PSSetShaderResources( slot, 1U, &srv );
}



//--------------------------------------------------------------------------
/**
* BindTextureView
*/
void RenderContext::BindTextureView( unsigned int slot, std::string const &filePath )
{
	TextureView2D* tv = CreateOrGetTextureViewFromFile( filePath.c_str() );
	BindTextureView( slot, tv ); 
}

//--------------------------------------------------------------------------
/**
* BindSampler
*/
void RenderContext::BindSampler( unsigned int slot, Sampler* sampler )
{
	if (sampler == nullptr) {
		sampler = m_samplers[SAMPLE_MODE_DEFAULT]; // bind the default if nothing is set
	}

	// create the dx handle; 
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState *handle = sampler->GetHandle(); 
	m_D3DContext->PSSetSamplers( slot, 1U, &handle );
}

//--------------------------------------------------------------------------
/**
* BindSampler
*/
void RenderContext::BindSampler( eSampleMode mode )
{
	Sampler* sampler = m_samplers[mode]; // bind the default if nothing is set	

	// create the dx handle; 
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState *handle = sampler->GetHandle(); 
	m_D3DContext->PSSetSamplers( 0, 1U, &handle ); 
}

//--------------------------------------------------------------------------
/**
* BindMaterial
*/
void RenderContext::BindMaterial( Material* material )
{
	m_material = material;
	if( !material )
	{
		BindShader( nullptr );
		for( unsigned int bindIdx = 0; bindIdx < NUM_TEXTURE_SLOTS; ++bindIdx )
		{
			BindTextureView( bindIdx, nullptr );
			BindSampler( bindIdx, nullptr );
		}
		return;
	}
	BindShader( material->m_shader );
	for( unsigned int bindIdx = 0; bindIdx < NUM_TEXTURE_SLOTS; ++bindIdx )
	{
		BindTextureView( bindIdx, material->m_textures[bindIdx] );
		BindSampler( bindIdx, material->m_samplers[bindIdx] );
	}
	BindUniformBuffer( material->m_slot, material->m_materialBuffer );
}

//--------------------------------------------------------------------------
/**
* BindMaterial
*/
void RenderContext::BindMaterial( const char* matPath, bool flipTextures /*= false*/ )
{
	BindMaterial( CreateOrGetMaterialFromXML( matPath, flipTextures ) );
}

//--------------------------------------------------------------------------
/**
* BindShader
*/
void RenderContext::BindShader( Shader* shader )
{
	if( shader == nullptr )
	{
		shader = CreateOrGetShaderFromSource( "DEFAULT", DEFAULT_SHADER_TEXT );
	}
	m_shader = shader;
	m_D3DContext->VSSetShader( shader->m_vertexShaderStage.m_vertexShader, nullptr, 0U );
	m_D3DContext->PSSetShader( shader->m_fragmentShaderStage.m_pixelShader, nullptr, 0U );
	shader->UpdateBlendStateIfDirty(); 
	m_D3DContext->OMSetDepthStencilState( shader->m_depthStencilState, 0U );
	float black[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
	m_D3DContext->OMSetBlendState( shader->m_blendState, // the d3d11 blend state object; 
		black,         // blend factor (some blend options use this) 
		0xffffffff );  // mask (what channels will get blended, this means ALL)
	m_D3DContext->RSSetState( shader->m_rasterizerState );

}

//--------------------------------------------------------------------------
/**
* SetBlendMode
*/
void RenderContext::SetBlendMode( eBlendMode mode )
{
	m_shader->SetBlendMode( mode );
}

//--------------------------------------------------------------------------
/**
* BindTextureViewWithSampler
*/
void RenderContext::BindTextureViewWithSampler( unsigned int slot, TextureView *view )
{
	BindTextureView( slot, view ); 

	if (view != nullptr) {
		BindSampler( slot, view->m_sampler );
	} else {
		BindSampler( slot, nullptr ); 
	}
}

//--------------------------------------------------------------------------
/**
* BindTextureViewWithSampler
*/
void RenderContext::BindTextureViewWithSampler( unsigned int slot, std::string const &filePath )
{
	TextureView2D* tv = CreateOrGetTextureViewFromFile( filePath.c_str() );
	BindTextureView( slot, tv ); 
	BindSampler( slot, tv->m_sampler );
}

//--------------------------------------------------------------------------
/**
* BindTextureViewWithSampler
*/
void RenderContext::BindTextureViewWithSampler( unsigned int slot, TextureView *view, Sampler *sampler )
{
	BindTextureView( slot, view ); 
	BindSampler( slot, sampler );
}

//--------------------------------------------------------------------------
/**
* BindTextureViewWithSampler
*/
void RenderContext::BindTextureViewWithSampler( unsigned int slot, TextureView *view, eSampleMode mode )
{
	BindTextureView( slot, view ); 
	BindSampler( mode );
}

//--------------------------------------------------------------------------
/**
* BindTextureViewWithSampler
*/
void RenderContext::BindTextureViewWithSampler( unsigned int slot, std::string const &filePath, eSampleMode mode )
{
	TextureView2D* tv = CreateOrGetTextureViewFromFile( filePath.c_str() );
	BindTextureView( slot, tv ); 
	BindSampler( mode );
}

//--------------------------------------------------------------------------
/**
* BindVertexStream
*/
void RenderContext::BindVertexStream( VertexBuffer *vbo )
{
	//m_immediateVBO = vbo;
	unsigned int stride = (unsigned int) vbo->m_stride;
	unsigned int offset = 0U;
	m_D3DContext->IASetVertexBuffers( 0,    // Start slot index
		1,                            // Number of buffers we're binding
		&vbo->m_handle, // Array of buffers
		&stride,                // Stride (read: vertex size, or amount we move forward each vertex) for each buffer
		&offset );             // Offset into each buffer (array - we are only passing one. 
}

//--------------------------------------------------------------------------
/**
* BindIndexStream
*/
void RenderContext::BindIndexStream( IndexBuffer* ibo )
{
	ID3D11Buffer *handle = nullptr; 
	if (ibo != nullptr) {
		handle = ibo->m_handle; 
	}

	m_D3DContext->IASetIndexBuffer( handle, 
		DXGI_FORMAT_R32_UINT,      // 32-bit indices;            
		0 );  // byte offset 
}

//--------------------------------------------------------------------------
/**
* BindUniformBuffer
*/
void RenderContext::BindUniformBuffer( unsigned int slot, UniformBuffer* ubo )
{
	// The API allows us to bind multiple constant buffers at once
	// and binds to each stage seperately.  For simplicity, we'll
	// just assume a slot is uniform for the entire pipeline
	ID3D11Buffer *buffer = (ubo != nullptr) ? ubo->m_handle : nullptr; 
	m_D3DContext->VSSetConstantBuffers( slot, 1U, &buffer ); 
	m_D3DContext->PSSetConstantBuffers( slot, 1U, &buffer ); 
}

//--------------------------------------------------------------------------
/**
* ClearColorTargets
*/
void RenderContext::ClearColorTargets( const Rgba& color )
{
	ClearScreen(color);
}

//--------------------------------------------------------------------------
/**
* ClearScreen
*/
void RenderContext::ClearScreen( const Rgba& clearColor )
{
	// Clear Buffer
	float clear_color[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	m_D3DContext->ClearRenderTargetView( m_camera->m_colorTargetView->m_rtv, clear_color );
}

//--------------------------------------------------------------------------
/**
* BeginCamera
*/
void RenderContext::BeginCamera( Camera* camera )
{
	m_camera = camera;
	Vec2 botLeft = m_camera->GetOrthoBottomLeft();
	Vec2 topRight = m_camera->GetOrthoTopRight();

	// Now, we don't actually render to anything by default - so before we can draw anything
	// we must tell D3D11 where we are rendering to.  In this case - the back buffer.
	m_D3DContext->OMSetRenderTargets( 1, &camera->m_colorTargetView->m_rtv, camera->m_depthTargetView->m_dsv );

	// Also, set which region of the screen we're rendering to, in this case, all of it 
	D3D11_VIEWPORT viewport;
	memset( &viewport, 0, sizeof(viewport) );
	viewport.TopLeftX = m_clientDims.x * camera->m_viewportInfo.topLeftPivot.x;
	viewport.TopLeftY = m_clientDims.y * camera->m_viewportInfo.topLeftPivot.y;
	viewport.Width = (float) m_clientDims.x * camera->m_viewportInfo.screenDims.x;
	viewport.Height = (float) m_clientDims.y * camera->m_viewportInfo.screenDims.y;
	viewport.MinDepth = camera->m_viewportInfo.depthMinMax.x;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = camera->m_viewportInfo.depthMinMax.y;        // must be between 0 and 1 (default is 1)
	m_D3DContext->RSSetViewports( 1, &viewport );

	m_camera->UpdateUniformBuffer( this );
	BindUniformBuffer( UNIFORM_SLOT_CAMERA, m_camera->m_cameraProjUBO ); 
	BindUniformBuffer( UNIFORM_SLOT_MODEL, m_modelUBO ); 
	BindUniformBuffer( UNIFORM_SLOT_FRAME, m_camera->m_timeUBO );
	BindUniformBuffer( UNIFORM_SLOT_LIGHTS, m_lightUBO );

	BindModelMatrix( Matrix44::IDENTITY ); 
}

//--------------------------------------------------------------------------
/**
* EndCamera
*/
void RenderContext::EndCamera()
{
	m_camera->SetColorTargetView( nullptr );
	m_camera->SetDepthTargetView( nullptr );
}

//--------------------------------------------------------------------------
/**
* GetColorTargetView
*/
ColorTargetView* RenderContext::GetColorTargetView()
{
	return m_colorTargetView;
}

//--------------------------------------------------------------------------
/**
* GetDepthTargetView
*/
DepthStencilTargetView* RenderContext::GetDepthTargetView()
{
	return m_defaultDSV;
}

//--------------------------------------------------------------------------
/**
* UpdateUniformBuffer
*/
void RenderContext::BindModelMatrix( const Matrix44& matrix )
{
	if (m_modelUBO == nullptr) 
	{
		m_modelUBO = new UniformBuffer( this ); 
	}

	ModelUBOData posData;
	posData.model = matrix; 

	m_modelUBO->CopyCPUToGPU( &posData, sizeof(posData) );
}

//--------------------------------------------------------------------------
/**
* DrawVertexArray
* Assumed its in sets of triangles
*/
void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{

	// copy to a vertex buffer
	m_immediateVBO->CopyCPUToGPU( vertexes, numVertexes );

	// bind that vertex buffer
	BindVertexStream( m_immediateVBO ); 

	Draw( numVertexes, 0, m_immediateVBO->m_layout ); 
}

//--------------------------------------------------------------------------
/**
* DrawVertexArray
*/
void RenderContext::DrawVertexArray( std::vector<Vertex_PCU> vertexes )
{
	DrawVertexArray( (int) vertexes.size(), &vertexes[0] );
}




//--------------------------------------------------------------------------
/**
* DrawMesh
*/
void RenderContext::DrawMesh( const MeshGPU* mesh )
{
	if ( m_lightBufferDirty ) {
		m_lightBufferDirty = false; 
		if( !m_lightUBO )
		{
			m_lightUBO = new UniformBuffer( this );
		}
		m_lightUBO->CopyCPUToGPU( &m_lightsData, sizeof(m_lightsData) ); 
	}

	BindVertexStream( mesh->m_vertexBuffer ); 
	BindIndexStream( mesh->m_indexBuffer ); 

	if (mesh->m_useIndexBuffer ) 
	{
		DrawIndexed( mesh->m_elementCount, mesh->m_vertexBuffer->m_layout ); 
	} else {
		Draw( (int) mesh->m_vertexBuffer->m_vertexCount, mesh->m_elementCount, mesh->m_vertexBuffer->m_layout ); 
	}
}

//--------------------------------------------------------------------------
/**
* DrawModel
*/
void RenderContext::DrawModel( const Model* model )
{
	BindModelMatrix( model->m_transformMatrix );
	for( int meshIdx = 0; meshIdx < (int) model->m_meshs.size(); ++meshIdx )
	{
		MeshGPU* mesh = model->m_meshs[meshIdx];
		if( mesh )
		{
			BindMaterial( model->m_materials[meshIdx] );
			DrawMesh( mesh );
		}
	}
}

//--------------------------------------------------------------------------
/**
* Draw
*/
void RenderContext::Draw( unsigned int vertCount, unsigned int byteOffset, const BufferAttribute* layout )
{
	// **NEW** - before a draw can happen, 
	// We need to describe the input to the shader
	// TODO: only create an input layout if the vertex type changes; 
	// TODO: When different vertex types come on-line, look at the current bound
	//       input streams (VertexBuffer) for the layout
	bool result = m_shader->CreateInputLayoutForBufferLayout( layout ); 


	if (result) {
		m_D3DContext->IASetInputLayout( m_shader->m_inputLayout );
		m_D3DContext->Draw( vertCount, byteOffset ); 
	} else {
		// error/warning
	}
}

//--------------------------------------------------------------------------
/**
* DrawIndexed
*/
void RenderContext::DrawIndexed( unsigned int indexCount, const BufferAttribute* layout )
{
	bool result = m_shader->CreateInputLayoutForBufferLayout( layout ); 

	if (result) {
		m_D3DContext->IASetInputLayout( m_shader->m_inputLayout );
		m_D3DContext->DrawIndexed( indexCount, 0, 0 ); 
	} else {
		// error/warning
	}
}

//--------------------------------------------------------------------------
/**
* SetAmbientLight
*/
void RenderContext::SetAmbientLight( const Rgba& color, float intensity )
{
	m_lightsData.ambient = color;
	m_lightsData.ambient.a = intensity;
	m_lightBufferDirty = true;
}

//--------------------------------------------------------------------------
/**
* EnableLight
*/
void RenderContext::EnableLight( unsigned int slot, const LightData& info )
{
	m_lightsData.lights[slot] = info;
	m_lightBufferDirty = true;
}

//--------------------------------------------------------------------------
/**
* DisableLight
*/
void RenderContext::DisableLight( unsigned int slot )
{
	m_lightsData.lights[slot].color.a = 0.0f;
	m_lightBufferDirty = true;
}

//--------------------------------------------------------------------------
/**
* GetLightAtSlot
*/
LightData RenderContext::GetLightAtSlot( unsigned int slot ) const
{
	return m_lightsData.lights[slot];
}

//--------------------------------------------------------------------------
/**
* SetSpecFactor
*/
void RenderContext::SetSpecFactor( float factor )
{
	m_lightsData.spec_factor = factor;
	m_lightBufferDirty = true;

}

//--------------------------------------------------------------------------
/**
* SetSpecPower
*/
void RenderContext::SetSpecPower( float power )
{
	m_lightsData.spec_power = power;
	m_lightBufferDirty = true;
}

//--------------------------------------------------------------------------
/**
* SetEmissiveFactor
*/
void RenderContext::SetEmissiveFactor( float factor )
{
	m_lightsData.emissive_factor = factor;
	m_lightBufferDirty = true;
}

//--------------------------------------------------------------------------
/**
* CopyTexture
*/
void RenderContext::CopyTexture( Texture* dst, Texture* src )
{
	m_D3DContext->CopyResource( dst->m_handle, src->m_handle );
	dst->m_memoryUsage = src->m_memoryUsage;
	dst->m_owner = src->m_owner;
	dst->m_dimensions = src->m_dimensions;
	dst->m_size = src->m_size;
}

//--------------------------------------------------------------------------
/**
* ApplyEffect
*/
void RenderContext::ApplyEffect( ColorTargetView *dst, TextureView *src, Material *mat )
{
	m_D3DContext->OMSetRenderTargets( 1, &dst->m_rtv, nullptr );
	BindMaterial( mat );
	BindTextureViewWithSampler( TEXTURE_SLOT_ALBEDO, src );

	D3D11_VIEWPORT viewport;
	memset( &viewport, 0, sizeof(viewport) );
	viewport.TopLeftX = 0U;
	viewport.TopLeftY = 0U;
	viewport.Width = (float) m_clientDims.x;
	viewport.Height = (float) m_clientDims.y;
	viewport.MinDepth = 0.0f;        
	viewport.MaxDepth = 1.0f;       
	m_D3DContext->RSSetViewports( 1, &viewport );

	m_D3DContext->IASetInputLayout( nullptr );
	m_D3DContext->Draw( 3, 0 );

	BindTextureViewWithSampler(TEXTURE_SLOT_ALBEDO, nullptr);
}

//--------------------------------------------------------------------------
/**
* GetScratchColorTargetView
*/
ColorTargetView* RenderContext::GetScratchColorTargetView()
{
	return m_scratchBuffer;
}

//--------------------------------------------------------------------------
/**
* GetScratchBuffer
*/
Texture* RenderContext::GetScratchBuffer()
{
	return (Texture*) m_scratchTexture;
}

//--------------------------------------------------------------------------
/**
* GetRenderTargetTextureView
*/
TextureView* RenderContext::GetRenderTargetTextureView()
{
	return m_ctvTextureView;
}

//--------------------------------------------------------------------------
/**
* GetBufferTexture
*/
Texture* RenderContext::GetBufferTexture()
{
	return (Texture2D*) m_ctvTexture;
}

//--------------------------------------------------------------------------
/**
* GetD3DDevice
*/
ID3D11Device* RenderContext::GetD3DDevice()
{
	return m_D3DDevice;
}

//--------------------------------------------------------------------------
/**
* GetD3DContext
*/
ID3D11DeviceContext* RenderContext::GetD3DContext()
{
	return m_D3DContext;
}

//--------------------------------------------------------------------------
/**
* GetWindowContext
*/
WindowContext* RenderContext::GetWindowContext()
{
	return m_windowContext;
}

//--------------------------------------------------------------------------
/**
* GetSamplerTypeFromString
*/
eSampleMode RenderContext::GetSamplerTypeFromString( std::string string )
{
	if( string == "linear" )
	{
		return SAMPLE_MODE_LINEAR;
	}
	if( string == "point" )
	{
		return SAMPLE_MODE_POINT;
	}
	return SAMPLE_MODE_DEFAULT;
}

//--------------------------------------------------------------------------
/**
* GetSampler
*/
Sampler* RenderContext::GetSampler( eSampleMode mode )
{
	return m_samplers[mode];
}

//--------------------------------------------------------------------------
/**
* CompileHLSLToShaderBlob
*/
ID3DBlob* RenderContext::CompileHLSLToShaderBlob( char const *opt_filename,	/* optional: used for error messages */
	void const *source_code,												/* buffer containing source code. */
	size_t const source_code_size,											/* size of the above buffer. */
	char const *entrypoint,													/* Name of the Function we treat as the entry point for this stage */
	char const* target )													/* What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)*/
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
#define DEBUG_SHADERS
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile( source_code, 
		source_code_size,                   // plain text source code
		opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compile_flags,                      // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors );                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*) errors->GetBufferPointer();
			ERROR_RECOVERABLE( Stringf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
				opt_filename, 
				error_string ) );
			DX_SAFE_RELEASE(errors);
		} else {
			ERROR_RECOVERABLE( "Failed to compile hr." );
		}
	} 
	DX_SAFE_RELEASE(errors);

	// will be nullptr if it failed to compile
	return code;
}

