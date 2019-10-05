#pragma once
#include <vector>
#include <map>
#include "Engine/Renderer/Shaders/ColorTargetView.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/XML/XMLUtils.hpp"


class Texture;
class Texture2D;
class TextureView2D;
class TextureView;
class BitmapFont;
class Shader;
class Sampler;
class ColorTargetView;
class VertexBuffer;
class IndexBuffer;
class WindowContext;
class MeshCPU;
class MeshGPU;
class Material;
class Model;

struct Rgba;
struct Vertex_PCU;
struct BufferAttribute;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RasterizerState;

struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

struct ID3D10Blob;




//--------------------------------------------------------------------------
class RenderContext
{
public:
	friend class Camera;
	friend class RenderBuffer;
	friend class UniformBuffer;
	friend class Shader;
	friend class Texture2D;
	friend class Sampler;
	friend class DepthStencilTargetView;
	friend class DebugRenderSystem;

public:
	RenderContext( WindowContext* displayHandle );
	~RenderContext();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();


	TextureView2D* CreateOrGetTextureViewFromFile( const char* imageFilePath, bool flip = false );
	BitmapFont* CreateOrGetBitmapFromFile( const char* fontName );
	MeshGPU* CreateOrGetMesh( const char* meshName );
	MeshGPU* CreateOrGetMesh( XmlElement* root );
	MeshCPU* CreateMesh( XmlElement* root );
	std::vector<MeshGPU*> CreateOrGetMeshWithSubmeshes( const char* meshName );
	Material* CreateOrGetMaterialFromXML( const char* matPath, bool flipTextures = false );
	Shader* CreateOrGetShaderFromFile( const char* filename ); 
	Shader* CreateOrGetShaderFromXML( const char* filename ); 
	Shader* CreateOrGetShaderFromSource( const char* filename, std::string source ); 
	Shader* GetCurShader() { return m_shader; }

	MeshGPU* RegisterMesh( MeshGPU* mesh, const std::string& name );
	MeshGPU* RegisterMesh( MeshCPU* mesh, const std::string& name );
	TextureView2D* RegisterTextureView( Texture2D* texture, const std::string& path );

public:
	void BindMaterial( Material* material );
	void BindMaterial( const char* matPath, bool flipTextures = false );
	void BindShader( Shader* shader );
	void SetBlendMode( eBlendMode mode );
	void BindModelMatrix( const Matrix44& matrix );

	void BindTextureView( unsigned int slot, const TextureView* view );
	void BindSampler( unsigned int slot, Sampler* sampler ); 

	// Some convenience
	void BindTextureView( unsigned int slot, std::string const &filePath ); 
	void BindSampler( eSampleMode mode );

	void BindTextureViewWithSampler( unsigned int slot, TextureView *view ); 
	void BindTextureViewWithSampler( unsigned int slot, std::string const &filePath ); 
	void BindTextureViewWithSampler( unsigned int slot, TextureView *view, Sampler *sampler ); 
	void BindTextureViewWithSampler( unsigned int slot, TextureView *view, eSampleMode mode ); 
	void BindTextureViewWithSampler( unsigned int slot, std::string const &filePath, eSampleMode mode ); 

	void BindVertexStream( VertexBuffer *vbo ); 
	void BindIndexStream( IndexBuffer* ibo );
	void BindUniformBuffer( unsigned int slot, UniformBuffer *ubo ); 

	void ClearColorTargets( const Rgba& color );

private:
	TextureView2D* CreateTextureViewFromFile( const char* imageFilePath, bool flip = false );
	BitmapFont* CreateBitmapFromFile( const char* fontName );
	Material* CreateMaterialFromXML( const char* filePath, bool flipTextures = false );
	Shader* CreateShaderFromFile( const char* filename );
	Shader* CreateShaderFromXML( const char* filename );
	Shader* CreateShaderFromSource( const char* filename, std::string source );

public:
	//Texture* CreateOrGetTextureViewFromFile( const char* imageFilePath );
	void ClearScreen( const Rgba& clearColor );
	void BeginCamera( Camera* camera );
	void EndCamera();
	Camera* GetCamera() const { return m_camera; }
	ColorTargetView* GetColorTargetView();
	DepthStencilTargetView* GetDepthTargetView();

	void DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void DrawVertexArray( std::vector<Vertex_PCU> vertexes );
	void DrawMesh( const MeshGPU* mesh );
	void DrawModel( const Model* model );

	void Draw( unsigned int vertCount, unsigned int byteOffset, const BufferAttribute* layout );
	void DrawIndexed( unsigned int indexCount, const BufferAttribute* layout );

public:
	void SetAmbientLight( const Rgba& color, float intensity );
	void EnableLight( unsigned int slot, const LightData& lightInfo );
	void DisableLight( unsigned int slot );
	LightData GetLightAtSlot( unsigned int slot ) const;
	void SetSpecFactor( float factor );
	void SetSpecPower( float power );
	void SetEmissiveFactor( float factor );

public:
	void CopyTexture( Texture* dst, Texture* src ); 
	void ApplyEffect( ColorTargetView* dst, TextureView* src, Material* mat ); 
	ColorTargetView* GetScratchColorTargetView();
	Texture* GetScratchBuffer();
	TextureView* GetRenderTargetTextureView();
	Texture* GetBufferTexture();

	ID3D11Device* GetD3DDevice();
	ID3D11DeviceContext* GetD3DContext();

	WindowContext* GetWindowContext();

public:
	static eSampleMode GetSamplerTypeFromString( std::string string );
	Sampler* GetSampler( eSampleMode mode );

private:
	std::map< std::string, TextureView2D* >	m_loadedTextureViews;
	std::map< std::string, BitmapFont* >	m_loadedFonts;
	std::map< std::string, Shader* >		m_loadedShaders;
	std::map< std::string, Material* >		m_loadedMaterials;
	std::map< std::string, MeshGPU* >		m_loadedMeshes;
	Sampler* m_samplers[SAMPLE_MODE_COUNT] = {};

	Camera* m_camera;
	IntVec2 m_clientDims;
	UniformBuffer* m_modelUBO		= nullptr;
	UniformBuffer* m_lightUBO		= nullptr;

	LightUBOData m_lightsData;
	bool m_lightBufferDirty			= true;

private:
	// D311 variables
	ID3D11Device *m_D3DDevice = nullptr;
	ID3D11DeviceContext *m_D3DContext = nullptr;
	IDXGISwapChain *m_D3DSwapChain = nullptr;

	
	ColorTargetView* m_scratchBuffer = nullptr;
	Texture2D* m_scratchTexture = nullptr;

	ColorTargetView* m_colorTargetView = nullptr;
	Texture2D* m_ctvTexture = nullptr;
	TextureView2D* m_ctvTextureView = nullptr;
	VertexBuffer* m_immediateVBO = nullptr; 
	MeshGPU* m_immediateMesh = nullptr;              

	WindowContext* m_windowContext = nullptr;

public:
	Shader* m_shader = nullptr;
	Shader* m_defaultShader = nullptr;
	Shader* m_DefaultUnlitShader = nullptr;
	Shader* m_InvalidShader = nullptr;

	Material* m_material = nullptr;

private:
	// Shader Functions
	ID3D10Blob* CompileHLSLToShaderBlob( char const *opt_filename,		/* optional: used for error messages*/
		void const *source_code,                                          /* buffer containing source code.*/
		size_t const source_code_size,                                    /* size of the above buffer.*/
		char const *entrypoint,                                           /* Name of the Function we treat as the entry point for this stage*/
		char const* target  );                                             /* What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)*/

private:
	DepthStencilTargetView* m_defaultDSV	= nullptr;

};




