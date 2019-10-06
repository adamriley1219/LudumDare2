
struct vs_input_t
{
	float3 position         : POSITION;
	float3 normal           : NORMAL;   
	float4 tangent		    : TANGENT;
	float4 color            : COLOR;
	float2 uv               : TEXCOORD; 
};



//--------------------------------------------------------------------------------------
cbuffer camera_constants : register(b2)
{
	float4x4 VIEW; 
	float4x4 PROJECTION; 
};

//--------------------------------------------------------------------------------------
cbuffer model_constants : register(b3)
{
	float4x4 MODEL;  // LOCAL_TO_WORLD

	float3 CAM_POS;
	float pad02;
}

struct light_t 
{
	float3 color; 
	float intensity; 

	float3 position; 
	float is_directional;

	float3 direction; 
	float pad10; 

	float3 diffuse_attenuation; 
	float pad20; 

	float3 specular_attenuation;
	float pad30; 
};


cbuffer light_buffer : register(b4) 
{
	float4 AMBIENT; 

	float SPEC_FACTOR;   // (0, 1), limits specular amount
	float SPEC_POWER; 
	float2 pad00; 

	light_t LIGHTS[8]; 
};


//--------------------------------------------------------------------------------------
Texture2D<float4> tAlbedo : register(t0); 
SamplerState sAlbedo : register(s0);      

										
struct v2f_t 
{
	float4 position : SV_POSITION; 
	float4 color : COLOR; 
	float2 uv : UV; 
}; 

//--------------------------------------------------------------------------------------
float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return ( v - inMin ) * ( outMax - outMin ) / ( inMax - inMin ) + outMin; 
}

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
	v2f_t v2f = (v2f_t)0;

	float4 local_pos = float4( input.position, 1.0f ); 
	float4 worldTangent = mul( MODEL, float4( input.tangent.xyz, 0.0f) );
	float4 world_pos = mul( MODEL, local_pos );
	float4 view_pos = mul( VIEW, world_pos ); 
	float4 clip_pos = mul( PROJECTION, view_pos ); 

	v2f.position = clip_pos; 
	v2f.color = float4( worldTangent.xyz, input.tangent.w );
	v2f.uv = input.uv; 

	return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	input.color.r = RangeMap( input.color.r, -1.0f, 1.0f, 0.0f, 1.0f );
	input.color.g = RangeMap( input.color.g, -1.0f, 1.0f, 0.0f, 1.0f );
	input.color.b = RangeMap( input.color.b, -1.0f, 1.0f, 0.0f, 1.0f );
	return input.color;
}
