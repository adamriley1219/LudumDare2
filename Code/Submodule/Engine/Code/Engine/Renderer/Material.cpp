#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Textures/TextureView.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/Shaders/UniformBuffer.hpp"
#include "Engine/Renderer/Textures/Texture2D.hpp"

//--------------------------------------------------------------------------
/**
* Material
*/
Material::Material( RenderContext *ctx )
{
	m_textures.resize( NUM_TEXTURE_SLOTS );
	m_samplers.resize( NUM_TEXTURE_SLOTS );
	m_renderContext = ctx;
	m_shader = m_renderContext->CreateOrGetShaderFromXML( "INVALID" );

	SetDiffuseMap(	( TextureView* ) ctx->CreateOrGetTextureViewFromFile( "white" ) ); 
	SetNormalMap(	( TextureView* ) ctx->CreateOrGetTextureViewFromFile( "flat" )  ); 
	SetEmissiveMap( ( TextureView* ) ctx->CreateOrGetTextureViewFromFile( "black" ) );
	SetSpecularMap( ( TextureView* ) ctx->CreateOrGetTextureViewFromFile( "white" ) );
	SetNoiseMap(	( TextureView* ) ctx->CreateOrGetTextureViewFromFile( "white" ) );
}

//--------------------------------------------------------------------------
/**
* Material
*/
Material::Material( RenderContext* ctx, XmlElement* xml, bool flipTexture /*= false*/ )
{
	m_textures.resize( NUM_TEXTURE_SLOTS );
	m_samplers.resize( NUM_TEXTURE_SLOTS );

	m_shader = ctx->CreateOrGetShaderFromXML( ParseXmlAttribute( *xml, "shader", "INVALID" ).c_str() );

	XmlElement* diffXml = xml->FirstChildElement( "diffuse" );
	XmlElement* normXml = xml->FirstChildElement( "normal" );
	XmlElement* emisXml = xml->FirstChildElement( "emissive" );
	XmlElement* specXml = xml->FirstChildElement( "specular" );
	XmlElement* noiseXml = xml->FirstChildElement( "noise" );

	std::string diffuseMap = "white";
	std::string normalMap = "flat";
	std::string emissiveMap = "black";
	std::string specularMap = "white";
	std::string noiseMap = "white";

	std::string diffuseMaptype = "linear";
	std::string normalMaptype = "linear";
	std::string emissiveMaptype = "linear";
	std::string specularMaptype = "linear";
	std::string noiseMaptype = "linear";

	if( diffXml )
	{
		diffuseMap = ParseXmlAttribute(  *diffXml, "src", diffuseMap );
		diffuseMaptype = ParseXmlAttribute( *diffXml, "samplerType", diffuseMaptype );
	}
	if( normXml )
	{
		normalMap = ParseXmlAttribute(   *normXml, "src", normalMap );
		normalMaptype = ParseXmlAttribute( *normXml, "samplerType", normalMaptype );
	}
	if( emisXml )
	{
		emissiveMap = ParseXmlAttribute( *emisXml, "src", emissiveMap );
		emissiveMaptype = ParseXmlAttribute( *emisXml, "samplerType", emissiveMaptype );
	}
	if( specXml )
	{
		specularMap = ParseXmlAttribute( *specXml, "src", specularMap );
		specularMaptype = ParseXmlAttribute( *specXml, "samplerType", specularMaptype );
	}
	if( noiseXml )
	{
		noiseMap = ParseXmlAttribute( *noiseXml, "src", noiseMap );
		noiseMaptype = ParseXmlAttribute( *noiseXml, "samplerType", noiseMaptype );
	}

	TextureView2D* diffView = ctx->CreateOrGetTextureViewFromFile( diffuseMap.c_str(), flipTexture );
	TextureView2D* normView = ctx->CreateOrGetTextureViewFromFile( normalMap.c_str(), flipTexture );
	TextureView2D* emisView = ctx->CreateOrGetTextureViewFromFile( emissiveMap.c_str(), flipTexture );
	TextureView2D* specView = ctx->CreateOrGetTextureViewFromFile( specularMap.c_str(), flipTexture );
	TextureView2D* noiseView = ctx->CreateOrGetTextureViewFromFile( noiseMap.c_str(), flipTexture );

	eSampleMode diffMode  = RenderContext::GetSamplerTypeFromString( diffuseMaptype );
	eSampleMode normMode  = RenderContext::GetSamplerTypeFromString( normalMaptype );
	eSampleMode emisMode  = RenderContext::GetSamplerTypeFromString( emissiveMaptype );
	eSampleMode specMode  = RenderContext::GetSamplerTypeFromString( specularMaptype );
	eSampleMode noiseMode = RenderContext::GetSamplerTypeFromString( noiseMaptype );

	diffView->SetSampler( ctx->GetSampler( diffMode ) );
	normView->SetSampler( ctx->GetSampler( normMode ) );
	emisView->SetSampler( ctx->GetSampler( emisMode ) );
	specView->SetSampler( ctx->GetSampler( specMode ) );
	noiseView->SetSampler( ctx->GetSampler( noiseMode ) );

	m_renderContext = ctx;
	SetDiffuseMap(	( TextureView* ) diffView ); 
	SetNormalMap(	( TextureView* ) normView ); 
	SetEmissiveMap( ( TextureView* ) emisView );
	SetSpecularMap( ( TextureView* ) specView );
	SetNoiseMap( ( TextureView* ) noiseView );
}

//--------------------------------------------------------------------------
/**
* Clone
*/
Material* Material::Clone( const Material& matierial )
{
	Material* mat = new Material( matierial.m_renderContext );
	mat->m_textures.resize( NUM_TEXTURE_SLOTS );
	mat->m_samplers.resize( NUM_TEXTURE_SLOTS );

	mat->m_shader		= matierial.m_shader;
	mat->m_samplers		= matierial.m_samplers;
	mat->m_textures		= matierial.m_textures;
	mat->m_slot			= matierial.m_slot;

	if( matierial.GetUniformBuffer() )
	{
		mat->m_materialBuffer = UniformBuffer::Clone( *matierial.GetUniformBuffer() );
	}

	return mat;
}

//--------------------------------------------------------------------------
/**
* ~Material
*/
Material::~Material()
{
	SAFE_DELETE( m_materialBuffer );
}

//--------------------------------------------------------------------------
/**
* SetShader
*/
void Material::SetShader( Shader *shader )
{
	m_shader = shader;
}

//--------------------------------------------------------------------------
/**
* SetShader
*/
void Material::SetShaderFromXML( char const* filePath )
{
	m_shader = m_renderContext->CreateOrGetShaderFromXML( filePath );
}

//--------------------------------------------------------------------------
/**
* SetTextureViewWithSampler
*/
void Material::SetTextureViewWithSampler( unsigned int slot, TextureView* view )
{
	SetTextureView( slot, view );
	if( view )
	{
		SetSampler( slot, view->m_sampler );
	}
	else
	{
		SetSampler( slot, nullptr );
	}
}

//--------------------------------------------------------------------------
/**
* SetTextureViewWithSampler
*/
void Material::SetTextureViewWithSampler( unsigned int slot, char const* filePath )
{
	SetTextureViewWithSampler( slot, (TextureView*) m_renderContext->CreateOrGetTextureViewFromFile( filePath ) );
}

//--------------------------------------------------------------------------
/**
* SetTextureView
*/
void Material::SetTextureView( unsigned int slot, TextureView* view )
{
	m_textures[slot] = view;
}

//--------------------------------------------------------------------------
/**
* SetTextureView
*/
void Material::SetTextureView( unsigned int slot, char const* filePath )
{
	SetTextureView( slot, (TextureView*) m_renderContext->CreateOrGetTextureViewFromFile( filePath ) );
}

//--------------------------------------------------------------------------
/**
* SetSampler
*/
void Material::SetSampler( unsigned int slot, Sampler* sampler )
{
	m_samplers[slot] = sampler;
}

//--------------------------------------------------------------------------
/**
* GetTextureView
*/
TextureView* Material::GetTextureView( unsigned int slot ) const
{
	return m_textures[slot];
}

//--------------------------------------------------------------------------
/**
* GetSampler
*/
Sampler* Material::GetSampler( unsigned int slot ) const
{
	return m_samplers[slot];
}

//--------------------------------------------------------------------------
/**
* SetDiffuseMap
*/
void Material::SetDiffuseMap( TextureView *view )
{
	SetTextureView( TEXTURE_SLOT_ALBEDO, view );
	SetSampler( TEXTURE_SLOT_ALBEDO, view->m_sampler );
}

//--------------------------------------------------------------------------
/**
* SetNormalMap
*/
void Material::SetNormalMap( TextureView* view )
{
	SetTextureView( TEXTURE_SLOT_NORMAL, view );
	SetSampler( TEXTURE_SLOT_NORMAL, view->m_sampler );
}

//--------------------------------------------------------------------------
/**
* SetEmissiveMap
*/
void Material::SetEmissiveMap( TextureView* view )
{
	SetTextureView( TEXTURE_SLOT_EMISSIVE, view );
	SetSampler( TEXTURE_SLOT_EMISSIVE, view->m_sampler );
}

//--------------------------------------------------------------------------
/**
* SetSpecularMap
*/
void Material::SetSpecularMap( TextureView* view )
{
	SetTextureView( TEXTURE_SLOT_SPECULAR, view );
	SetSampler( TEXTURE_SLOT_SPECULAR, view->m_sampler );
}

//--------------------------------------------------------------------------
/**
* SetNoiseMap
*/
void Material::SetNoiseMap( TextureView* view )
{
	SetTextureView( TEXTURE_SLOT_NOISE, view );
	SetSampler( TEXTURE_SLOT_NOISE, view->m_sampler );
}

//--------------------------------------------------------------------------
/**
* SetUniforms
*/
void Material::SetUniforms( void const *data, size_t const size )
{
	if( !m_materialBuffer )
	{
		m_materialBuffer = new UniformBuffer( m_renderContext );
	}
	m_materialBuffer->CopyCPUToGPU( data, size );
}

//--------------------------------------------------------------------------
/**
* SetUniforms
*/
void Material::SetUniforms( UniformBuffer *ubo )
{
	m_materialBuffer = ubo;
}

//--------------------------------------------------------------------------
/**
* GetUniformBuffer
*/
UniformBuffer* Material::GetUniformBuffer() const
{
	return m_materialBuffer;
}
