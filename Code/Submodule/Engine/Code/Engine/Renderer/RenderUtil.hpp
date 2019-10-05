#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"

//------------------------------------------------------------------------
// MACROS
//------------------------------------------------------------------------
#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }
#define MemZero(obj) memset( &obj, 0, sizeof(obj) );


//--------------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------------

enum eCompareOp            
{
	COMPARE_NEVER,       // false,      "never"
	COMPARE_ALWAYS,      // true,       "always"
	COMPARE_EQUAL,       // ==,         "equal"
	COMPARE_NOTEQUAL,    // !=,         "not"
	COMPARE_LESS,        // <           "less"
	COMPARE_LEQUAL,      // <=          "lequal"
	COMPARE_GREATER,     // >           "greater"
	COMPARE_GEQUAL,      // >=          "gequal"
}; 


enum eGPUMemoryUsage 
{
	GPU_MEMORY_USAGE_GPU,     // Can be written/read from GPU only (Color Targets are a good example)
	GPU_MEMORY_USAGE_STATIC,  // Created, and are read only after that (ex: textures from images, sprite atlas)
	GPU_MEMORY_USAGE_DYNAMIC, // Update often from CPU, used by the GPU (CPU -> GPU updates, used by shaders.  ex: Uniform Buffers)
	GPU_MEMORY_USAGE_STAGING, // For getting memory from GPU to CPU (can be copied into, but not directly bound as an output.  ex: Screenshots system)
}; 


enum eRenderBufferUsageBit : unsigned int
{
	RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT   = BIT_FLAG(0),   // Can be bound to an vertex input stream slot
	RENDER_BUFFER_USAGE_INDEX_STREAM_BIT    = BIT_FLAG(1),   // Can be bound as an index input stream.  
	RENDER_BUFFER_USAGE_UNIFORMS_BIT        = BIT_FLAG(2),   // Can be bound to a constant buffer slot; 
};
typedef unsigned int eRenderBufferUsageBits; 

enum eFilterMode 
{
	FILTER_MODE_POINT,
	FILTER_MODE_LINEAR,
}; 

enum eCoreUniformSlot 
{
	UNIFORM_SLOT_FRAME		= 1,
	UNIFORM_SLOT_CAMERA		= 2, 
	UNIFORM_SLOT_MODEL		= 3,
	UNIFORM_SLOT_LIGHTS		= 4,
	UNIFORM_SLOT_ENGINE1	= 5,
	UNIFORM_SLOT_ENGINE2	= 6,
	UNIFORM_SLOT_ENGINE3	= 7,
//	Game buffer's start at slot 8
}; 

enum eBlendMode
{
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE,
	BLEND_MODE_OPAQUE,
	NUM_BLEND_MODES
};

enum eRasterCullMode
{
	CULL_FRONT,
	CULL_BACK,
	CULL_NONE,
	NUM_CULL_MODES
};

enum eSampleMode
{
	SAMPLE_MODE_POINT       = 0, 
	SAMPLE_MODE_LINEAR, 
	// SAMPLE_MODE_BILINEAR

	SAMPLE_MODE_COUNT,
	SAMPLE_MODE_DEFAULT     = SAMPLE_MODE_LINEAR // different games may want to change this.  My projects will use Linear, SD4 will want point; 
}; 

enum eTextureSlot
{
	TEXTURE_SLOT_ALBEDO,
	TEXTURE_SLOT_NORMAL,
	TEXTURE_SLOT_EMISSIVE,
	TEXTURE_SLOT_SPECULAR,
	TEXTURE_SLOT_NOISE,
	NUM_TEXTURE_SLOTS
};

//--------------------------------------------------------------------------
// Forward Declarations
//--------------------------------------------------------------------------
enum D3D11_USAGE : int;
enum D3D11_FILTER : int;
enum D3D11_COMPARISON_FUNC : int;
enum D3D11_CULL_MODE : int;
enum D3D11_FILL_MODE : int;

//--------------------------------------------------------------------------
// Methods
//--------------------------------------------------------------------------



//------------------------------------------------------------------------
// translate external enum to D3D11 specific options; 
// These will be used by texture as well,
// so may want to move this function to a common include; 
D3D11_USAGE DXUsageFromMemoryUsage( eGPUMemoryUsage const usage );

//------------------------------------------------------------------------
// Convert a buffer usage bitfield to a DX specific 
// version; 
unsigned int DXBufferUsageFromBufferUsage( eRenderBufferUsageBits const usage );

//------------------------------------------------------------------------
D3D11_FILTER DXGetFilter( eFilterMode min, eFilterMode mag );

D3D11_COMPARISON_FUNC DXGetCompareFunc( eCompareOp op );

D3D11_CULL_MODE DXGetCullMode( eRasterCullMode mode );

D3D11_FILL_MODE DXGetFillMode( bool fill );

struct CameraUBOData
{
	Matrix44 view; 
	Matrix44 proj; 
	Vec3 camPos;
	float pad00; 
};

struct ModelUBOData
{
	Matrix44 model;
};

struct frameBufferData
{
	float time; 
	float cosTime; 
	float worldWidth;
	float worldHeight;
}; 

struct ProjViewData
{
	Matrix44 m_view;
	Matrix44 m_proj;
};

struct LightData
{
	Rgba color				 = Rgba( 1.0f, 1.0f, 1.0f, 0.0f ); // a is intensity; 

	Vec3 position			 = Vec3::ZERO;       // where is the light
	float pad00 = 0.0f;                     // keep a 16-byte alignment for D3D11

	Vec3 direction           = Vec3( 0.0f, 0.0f, 1.0f ); // which way is the light facing
	float is_direction       = 0.0f; // 0 means use as a point light 

	Vec3 diffuse_attenuation = Vec3( 1.0f, 0.1f, 0.0f );  // no attenuation
	float pad10 = 0.0f; 

	Vec3 specular_attenuation = Vec3( 1.0f, 0.0f, 0.0f ); // no attenuation
	float pad20 = 0.0f; 
};

struct LightUBOData
{
	Rgba ambient = Rgba::FADED_GRAY; 
	float spec_factor = 1.0f; 
	float spec_power = 16.0f; 
	float emissive_factor = 0.0f;
	float pad; 

	LightData lights[MAX_LIGHTS]; 
};