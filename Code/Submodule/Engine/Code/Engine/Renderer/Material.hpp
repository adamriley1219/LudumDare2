#pragma once
#include <vector>
#include "Engine/Core/XML/XMLUtils.hpp"

class RenderContext;
class Shader;
class TextureView;
class UniformBuffer;
class Sampler;

class Material
{
	friend class RenderContext;
public:
	Material( RenderContext* ctx ); 
	Material( RenderContext* ctx, XmlElement* xml, bool flipTexture = false );
	Material( const Material& ) = delete;
	static Material* Clone( const Material& matierial );
	~Material();

	void SetShader( Shader* shader ); 
	void SetShaderFromXML( char const* filePath ); 

	void SetTextureViewWithSampler( unsigned int slot, TextureView* view ); 
	void SetTextureViewWithSampler( unsigned int slot, char const* filePath );

	void SetTextureView( unsigned int slot, TextureView* view ); 
	void SetTextureView( unsigned int slot, char const* filePath );

	void SetSampler( unsigned int slot, Sampler* sampler ); 

	TextureView* GetTextureView( unsigned int slot ) const; 
	Sampler* GetSampler( unsigned int slot ) const; 

	// Convenience Helpers - meaning it can pick a sane default when set to nullptr;
	void SetDiffuseMap( TextureView* view );  // default to "white" on nullptr;
	void SetNormalMap( TextureView* view );   // default to "flat" on nullptr;
	void SetEmissiveMap( TextureView* view ); // default to "black" on nullptr; 
	void SetSpecularMap( TextureView* view ); // default to "white" on nullptr;
	void SetNoiseMap( TextureView* view ); // default to "white" on nullptr;

	void SetUniforms( void const *data, size_t const size );
	void SetUniforms( UniformBuffer *ubo ); 
	UniformBuffer* GetUniformBuffer() const;

	// templated SetUniforms could be useful as well
	// ...

private:
	Shader* m_shader; 
	std::vector<TextureView*> m_textures; 
	std::vector<Sampler*> m_samplers; 

	UniformBuffer* m_materialBuffer     = nullptr; 
	unsigned int   m_slot				= 8;
	bool		   m_UBOdirty			= false;

	RenderContext* m_renderContext		= nullptr;
};
