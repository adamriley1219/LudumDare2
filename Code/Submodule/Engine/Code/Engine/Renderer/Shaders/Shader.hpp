#pragma once
#include "ShaderStage.hpp"
#include "Engine/Renderer/RenderUtil.hpp"

struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct ID3D10Blob;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct BufferAttribute;

//--------------------------------------------------------------------------
class Shader
{
public:
	friend class RenderContext;

	Shader( std::string name, RenderContext* owner ){ m_name = name; m_owner = owner; }
	Shader( Shader* shader );
	~Shader();

	void SetBlendMode( eBlendMode mode );
	void SetBlendMode( std::string mode );

	bool CreateInputLayoutForBufferLayout( const BufferAttribute* layout ); 
	bool UpdateInputLayout( const BufferAttribute* layout );
	bool UpdateBlendStateIfDirty(); 

	void SetDepthState( eCompareOp op, bool write );
	void SetDepthState( std::string op, bool write );

	void SetRasterState( eRasterCullMode cullmode, bool wireFrame = false, bool clockwise = false );
	void SetRasterState( std::string cullmode, bool wireFrame = false, bool clockwise = false );

public:
	std::string m_name = "";

private:
	eBlendMode StringToBlendMode( std::string mode );
	eCompareOp StringToCompareOp( std::string op );
	eRasterCullMode StringToCullMode( std::string cullmode );

private:
	ShaderStage m_vertexShaderStage;
	ShaderStage m_fragmentShaderStage;

	RenderContext* m_owner = nullptr;

	bool m_blendStateDirty					= true; 
	bool m_depthStateDirty					= true;      
	bool m_rasterStateDirty					= true;

	eBlendMode m_blendMode					= BLEND_MODE_ALPHA; 

	eCompareOp m_depthCompareOp				= COMPARE_LEQUAL;    
	bool m_writeDepth						= false;

	eRasterCullMode	m_cullMode				= CULL_BACK;
	bool m_wireframe						= false;
	bool m_clockwise						= false;

private:
	const BufferAttribute* m_currentLayout			= nullptr;
	ID3D11InputLayout* m_inputLayout				= nullptr; 
	ID3D11BlendState* m_blendState					= nullptr; 
	ID3D10Blob* m_VSbytecode						= nullptr; 
	ID3D11DepthStencilState* m_depthStencilState	= nullptr;    
	ID3D11RasterizerState* m_rasterizerState		= nullptr;

};
