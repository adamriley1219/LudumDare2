#define MAX_LIGHTS (8)

//--------------------------------------------------------------------------------------
// GLOBELS
//--------------------------------------------------------------------------------------

static const float GAMMA  = 2.0f;

//--------------------------------------------------------------------------------------
// STRUCTS
//--------------------------------------------------------------------------------------
struct vs_input_t
{
   float3 position         : POSITION;
   float3 normal           : NORMAL;   
   float4 tangent		   : TANGENT;
   float4 color            : COLOR;
   float2 uv               : TEXCOORD; 
};

//--------------------------------------------------------------------------------------
struct light_t 
{
   float3 color; 
   float intensity; 

   float3 position; 
   float pad10; 

   float3 direction; 
   float is_directional;

   float3 diffuse_attenuation; 
   float pad20; 

   float3 specular_attenuation;
   float pad30; 
};

//--------------------------------------------------------------------------------------
struct lighting_t
{
	float3 diffuse; 
	float pad01;
	float3 specular;
	float pad11;
};

//--------------------------------------------------------------------------------------
struct v2f_t 
{
	float4 position : SV_POSITION; 
	float4 color : COLOR;

	float2 uv : UV; 
	float2 pad14 : PAD14;
	
	float3 normal : NORMAL;
	float pad04 : PAD04;

	float3 tangent : TANGENT;
	float pad34	: PAD34;

	float3 bitangent : BITANGENT;
	float pad44	: PAD44;

	float3 worldPos : WORLDPOS;
	float pad24 : PAD24;
}; 

//--------------------------------------------------------------------------------------
// CBUFFERS
//--------------------------------------------------------------------------------------
cbuffer camera_constants : register(b2)
{
   float4x4 VIEW;
   float4x4 PROJECTION;
   float3 CAM_POS;
   float pad02;
};

//--------------------------------------------------------------------------------------
cbuffer model_constants : register(b3)
{
	float4x4 MODEL;  // LOCAL_TO_WORLD


}

//--------------------------------------------------------------------------------------
cbuffer light_buffer : register(b4) 
{
   float4 AMBIENT; 

   float SPEC_FACTOR;   // (0, 1), limits specular amount
   float SPEC_POWER; 
   float EMISSIVE_FACTOR;
   float pad03; 

   light_t LIGHTS[MAX_LIGHTS]; 
};


//---------------------------------------------------------------------------------------------
// Textures
//---------------------------------------------------------------------------------------------
Texture2D<float4> tAlbedo : register(t0); // texutre I'm using for albedo (color) information
SamplerState sAlbedo : register(s0);      // sampler I'm using for the Albedo texture

Texture2D<float4> tNormal : register(t1);
SamplerState sNormal : register(s1);     

Texture2D<float4> tEmissive : register(t2); 
SamplerState sEmissive : register(s2);      



//--------------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------------
float RangeMap( float v, float inMin, float inMax, float outMin, float outMax ) 
{ 
	return ( v - inMin ) * ( outMax - outMin ) / ( inMax - inMin ) + outMin; 
}

//--------------------------------------------------------------------------------------
// Blin-Phong
//--------------------------------------------------------------------------------------
lighting_t GetLighting( float3 eye_pos, 
   float3 surface_position, 
   float3 surface_normal )
{
   lighting_t lighting; 
   lighting.diffuse = (AMBIENT.xyz * AMBIENT.w); 

   float3 dir_to_eye = normalize(eye_pos - surface_position); 


   // directional light; 
   for (int i = 0; i < MAX_LIGHTS; ++i) {
      light_t light = LIGHTS[i]; 

      // directional 
      float3 dir_dir = light.direction; 
      float3 point_dir = normalize(surface_position - light.position); 
      float3 light_dir = lerp( point_dir, dir_dir, light.is_directional ); 

      // common things
      // directional light
      float dir_dist = abs( dot( (surface_position - light.position), light.direction ) );   // for directional
      float point_dist = length( surface_position - light.position );                          // for point
      float distance = lerp( point_dist, dir_dist, light.is_directional ); 

      // Diffuse Part
      float3 la = light.diffuse_attenuation; 
      float attenuation = 1.0f / (la.x + la.y * distance + la.z * distance * distance); 
      float dot3 = max( dot( -1.0f * light_dir, surface_normal ), 0.0f ); 

      float3 diffuse_color = light.color * light.intensity * attenuation * dot3; 
	  lighting.diffuse += diffuse_color; 

      // Specular 
      // blinn-phong 
      // dot( H, N );  -> H == half_vector, N == normal
      float3 dir_to_light = -light_dir; 
      float3 half_vector = normalize( dir_to_eye + dir_to_light ); 
      float spec_coefficient = max( dot( half_vector, surface_normal ), 0.0f ); // DO not saturate - spec can go higher;  

      float3 sa = light.specular_attenuation; 
      float spec_attenuation = 1.0f / (sa.x + sa.y * distance + sa.z * distance * distance); 

      // finalize coefficient
      spec_coefficient = SPEC_FACTOR * pow( spec_coefficient, SPEC_POWER ); 
      float3 specular_color = light.color * light.intensity * spec_attenuation * spec_coefficient; 
	  lighting.specular += specular_color; 
   }

   lighting.diffuse = saturate(lighting.diffuse); // clamp this to (0, 1)
   // light.specular is untouched - greater than one can tell us information on how bright it is - used for bloom;  

   return lighting; 
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
v2f_t VertexFunction(vs_input_t input)
{
	v2f_t v2f = (v2f_t)0;
	float4 local_pos = float4( input.position, 1.0f ); 
	float4 worldNormal = mul( MODEL, float4( input.normal, 0.0f) );
	float4 worldTangent = mul( MODEL, float4( input.tangent.xyz, 0.0f) );
	float4 worldBitangent = mul( MODEL, float4( cross( input.tangent.xyz, input.normal ) * input.tangent.w , 0.0f) );
	float4 world_pos = mul( MODEL, local_pos );
	float4 view_pos = mul( VIEW, world_pos ); 
	float4 clip_pos = mul( PROJECTION, view_pos ); 

	v2f.position = clip_pos; 
	v2f.color = input.color;
	v2f.normal = worldNormal.xyz;
	v2f.tangent = worldTangent.xyz;
	v2f.bitangent = worldBitangent.xyz;
	v2f.worldPos = world_pos.xyz;
	v2f.uv = input.uv; 

	return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
//--------------------------------------------------------------------------------------
float4 FragmentFunction( v2f_t input ) : SV_TARGET0
{
	input.normal = normalize( input.normal );
	input.tangent = normalize( input.tangent );
	input.bitangent = normalize( input.bitangent );
	

	float4 texColor = tAlbedo.Sample( sAlbedo, input.uv ); 
	float4 normal_color = tNormal.Sample( sNormal, input.uv );

	// shorthand - rangemap (0, 0, 0) to (1, 1, 1) -> (-1, -1, 0) to (1, 1, 1)
	float3 surface_normal = normal_color.xyz * float3(2, 2, 1) - float3(1, 1, 0); 

	// commonly referred to the TBN matrix
	float3x3 surface_to_world = float3x3( input.tangent, input.bitangent, input.normal );

	float3 world_normal = mul( surface_normal, surface_to_world ); 

	float4 surface_color = input.color * texColor; // from texture; 
	surface_color = pow( abs(surface_color), GAMMA );  // move to linear space; 

	lighting_t lighting = GetLighting( CAM_POS, input.worldPos.xyz, world_normal ); 

	float4 final_color = float4(lighting.diffuse, 1.0f) * surface_color
		+ float4(lighting.specular, 0.0f); 

	// EMISSIVE (map defaults to "black"); 
	float4 emissive = tEmissive.Sample( sEmissive, input.uv ) * EMISSIVE_FACTOR; 
	final_color += float4( emissive.xyz * emissive.w, 0 );

	final_color = pow( abs(final_color), 1.0f / GAMMA ); // convert back to sRGB space
	return final_color; 
};
