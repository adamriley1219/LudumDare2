#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Core/Vertex/VertexMaster.hpp"
#include <d3d11.h>  
#include <DXGI.h>   
#include <dxgidebug.h>


//--------------------------------------------------------------------------
/**
* Shader
*/
Shader::Shader( Shader* shader )
{
	m_name							= shader->m_name;
	m_vertexShaderStage				= shader->m_vertexShaderStage;
	m_vertexShaderStage.m_handle->AddRef();
	m_fragmentShaderStage			= shader->m_fragmentShaderStage;
	m_fragmentShaderStage.m_handle->AddRef();

	m_owner							= shader->m_owner;

	m_blendMode						= shader->m_blendMode; 
	m_blendStateDirty				= shader->m_blendStateDirty; 
	m_depthStateDirty				= shader->m_depthCompareOp;           


	m_depthCompareOp				= shader->m_depthCompareOp; 
	m_writeDepth					= shader->m_writeDepth;

	m_inputLayout					= shader->m_inputLayout;
	m_blendState					= shader->m_blendState;
	m_VSbytecode					= shader->m_VSbytecode; 
	m_depthStencilState				= shader->m_depthStencilState;
	m_rasterizerState				= shader->m_rasterizerState;
}

//--------------------------------------------------------------------------
/**
* ~Shader
*/
Shader::~Shader()
{
	DX_SAFE_RELEASE( m_inputLayout );
	DX_SAFE_RELEASE( m_blendState );
	DX_SAFE_RELEASE( m_VSbytecode );
	DX_SAFE_RELEASE( m_depthStencilState );
	DX_SAFE_RELEASE( m_rasterizerState );
}

//--------------------------------------------------------------------------
/**
* SetBlendMode
*/
void Shader::SetBlendMode( eBlendMode mode )
{
	m_blendMode = mode;
	m_blendStateDirty = true;
	
}

//--------------------------------------------------------------------------
/**
* SetBlendMode
*/
void Shader::SetBlendMode( std::string mode )
{
	SetBlendMode( StringToBlendMode( mode ) );
}

//--------------------------------------------------------------------------
/**
* CreateInputLayoutForBufferLayout
*/
bool Shader::CreateInputLayoutForBufferLayout( const BufferAttribute* layout )
{
	// Early out - we've already created it; 
	// TODO: If vertex type changes, we need to rebind; 
	if (m_inputLayout != nullptr) {
		return true; 
	}

	// This describes the input data to the shader
	// The INPUT_ELEMENT_DESC describes each element of the structure; 
	// Since we have POSITION, COLOR, UV, we need three descriptions; 

	return UpdateInputLayout( layout );
	
}

//--------------------------------------------------------------------------
/**
* GetDXGIFromRenderDataFormat
*/
static DXGI_FORMAT GetDXGIFromRenderDataFormat( eRenderDataFormat format )
{
	switch( format )
	{
	case RDF_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
		break;
	case RDF_VEC2:
		return DXGI_FORMAT_R32G32_FLOAT;
		break;
	case RDF_VEC3:
		return DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case RDF_VEC4:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case RDF_RGBA32:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	default:
		break;
	}
	return DXGI_FORMAT_R32_FLOAT;
}


//--------------------------------------------------------------------------
/**
* UpdateInputLayout
*/
bool Shader::UpdateInputLayout( const BufferAttribute* layout )
{
	if( m_currentLayout == layout )
	{
		return true;
	}

	size_t numAtt = ComputeAttBufferCount( layout );
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescriptions;

	for( unsigned int attIndx = 0; attIndx < numAtt; ++attIndx )
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = layout[attIndx].name.c_str();            
		desc.SemanticIndex = 0;                    
		desc.Format = GetDXGIFromRenderDataFormat( layout[attIndx].format ); 
		desc.InputSlot = 0U;                        
		desc.AlignedByteOffset = (UINT)layout[attIndx].memberOffset; 
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;  
		desc.InstanceDataStepRate = 0U;      
		inputDescriptions.push_back( desc );
	}

	ID3D10Blob *vs_bytecode = m_VSbytecode; 
	DX_SAFE_RELEASE( m_inputLayout );

	// Final create the layout
	HRESULT hr = m_owner->m_D3DDevice->CreateInputLayout( &inputDescriptions[0], 
		(UINT)numAtt,
		vs_bytecode->GetBufferPointer(), 
		vs_bytecode->GetBufferSize(), 
		&m_inputLayout );   

	m_currentLayout = layout;
	return SUCCEEDED(hr); 
}

//--------------------------------------------------------------------------
/**
* UpdateBlendStateIfDirty
*/
bool Shader::UpdateBlendStateIfDirty()
{
	// if we already have a good blend state - keep it; 
	if ((m_blendState != nullptr) && (false == m_blendStateDirty) && (!m_rasterStateDirty) && !m_depthStateDirty) {
		return true; 
	}

	// Free old state
	DX_SAFE_RELEASE(m_blendState); 

	// otherwise, let's make one; 
	D3D11_BLEND_DESC desc;
	MemZero( desc ); 

	desc.AlphaToCoverageEnable = false;  // used in MSAA to treat alpha as coverage (usual example is foliage rendering, we will not be using this)
	desc.IndependentBlendEnable = false;   // if you have multiple outputs bound, you can set this to true to have different blend state per output

	// Blending is setting put the equation...
	// FinalColor = BlendOp( SrcFactor * outputColor, DestFactor * destColor )
	// where outputColor is what the pixel shader outputs
	// and destColor is the color already in the pixel shader

	// the below describes the equation...
	// FinalColor = outputColor.a * outputColor + (1.0f - outputColor.a) * destColor;  

	// since we disabled independent blend, we only have to setup the first blend state
	// and I'm setting it up for "alpha blending"
	desc.RenderTarget[0].BlendEnable = TRUE;  // we want to blend

	if (m_blendMode == BLEND_MODE_ALPHA) {
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_SRC_ALPHA;      // output color is multiplied by the output colors alpha and added to...
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_INV_SRC_ALPHA;  // the current destination multiplied by (1 - output.a); 
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;        // we add the two results together

																	   // you can compute alpha seperately, in this case, we'll just set it to be the max alpha between the src & destination
		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
	} 
	else if (m_blendMode == BLEND_MODE_ADDITIVE)
	{
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_ONE;     
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_ONE;  
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;        


		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
	}
	else if(m_blendMode == BLEND_MODE_OPAQUE)
	{
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_ONE;     
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_ZERO;  
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;        


		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	}
	else { 
		// TODO: Add else_if branches for the other blend modes; 
		ASSERT_RECOVERABLE(false, "Shader::UpdateBlendStateIfDirty - UNIMPLEMENTED blendmode." ); // Unimplemented blend mode; 
					   // probably need to add the other cases
	};

	desc.RenderTarget[0].RenderTargetWriteMask       = D3D11_COLOR_WRITE_ENABLE_ALL;  // can mask off outputs;  we won't be doing that; 

																					  // Finally, create the blend state
	m_owner->m_D3DDevice->CreateBlendState( &desc, &m_blendState );

	if (m_depthStateDirty || (m_depthStencilState == nullptr)) 
	{
		D3D11_DEPTH_STENCIL_DESC ds_desc = {};

		ds_desc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and comprae is always)
		ds_desc.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO; 
		ds_desc.DepthFunc = DXGetCompareFunc( m_depthCompareOp ); //  


		ds_desc.StencilEnable = false; 
		ds_desc.StencilReadMask = 0xFF; 
		ds_desc.StencilWriteMask = 0xFF; 

		D3D11_DEPTH_STENCILOP_DESC default_stencil_op = {}; 
		default_stencil_op.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
		default_stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
		default_stencil_op.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
		default_stencil_op.StencilFunc = D3D11_COMPARISON_ALWAYS;      // function to test against

																	   // can have different rules setup for front and backface
		ds_desc.FrontFace = default_stencil_op; 
		ds_desc.BackFace = default_stencil_op; 

		DX_SAFE_RELEASE(m_depthStencilState); 
		m_owner->m_D3DDevice->CreateDepthStencilState( &ds_desc, &m_depthStencilState ); 
		m_depthStateDirty = false; 
	}

	if( m_rasterStateDirty )
	{
		D3D11_RASTERIZER_DESC rasterDesc;

		rasterDesc.FillMode = DXGetFillMode( !m_wireframe );
		rasterDesc.CullMode = DXGetCullMode( m_cullMode );
		rasterDesc.FrontCounterClockwise = !m_clockwise; 

		rasterDesc.DepthBias = 0U; 
		rasterDesc.DepthBiasClamp = 0.0f; 
		rasterDesc.SlopeScaledDepthBias = 0.0f; 
		rasterDesc.DepthClipEnable = TRUE; 
		rasterDesc.ScissorEnable = FALSE; 
		rasterDesc.MultisampleEnable = FALSE; 
		rasterDesc.AntialiasedLineEnable = FALSE; 

		DX_SAFE_RELEASE( m_rasterizerState );
		m_owner->m_D3DDevice->CreateRasterizerState( &rasterDesc, &m_rasterizerState );

		m_rasterStateDirty = false;
	}

	m_blendStateDirty = false; 
	return (m_blendState != nullptr); 
}

//--------------------------------------------------------------------------
/**
* SetDepthState
*/
void Shader::SetDepthState( eCompareOp op, bool write )
{
	m_depthStateDirty = true;
	m_depthCompareOp = op;
	m_writeDepth = write;
}

//--------------------------------------------------------------------------
/**
* SetDepthState
*/
void Shader::SetDepthState( std::string op, bool write )
{
	SetDepthState( StringToCompareOp( op ), write );
}

//--------------------------------------------------------------------------
/**
* SetRasterState
*/
void Shader::SetRasterState( eRasterCullMode cullmode, bool wireFrame /*= false*/, bool clockwise /*= false */ )
{
	m_cullMode = cullmode;
	m_wireframe = wireFrame;
	m_clockwise = clockwise;
	m_rasterStateDirty = true;
}

//--------------------------------------------------------------------------
/**
* SetRasterState
*/
void Shader::SetRasterState( std::string cullmode, bool wireFrame /*= false*/, bool clockwise /*= false */ )
{
	SetRasterState( StringToCullMode( cullmode ), wireFrame, clockwise );
}

//--------------------------------------------------------------------------
/**
* StringToBlendMode
*/
eBlendMode Shader::StringToBlendMode( std::string mode )
{
	if( mode == "alpha" )
	{
		return BLEND_MODE_ALPHA;
	}
	if( mode == "additive" )
	{
		return BLEND_MODE_ADDITIVE;
	}
	if( mode == "opaque" )
	{
		return BLEND_MODE_OPAQUE;
	}
	return BLEND_MODE_ALPHA;
}

//--------------------------------------------------------------------------
/**
* StringToCompareOp
*/
eCompareOp Shader::StringToCompareOp( std::string op )
{
	if( op == "never" )
	{
		return COMPARE_NEVER;
	}
	if( op == "always" )
	{
		return COMPARE_ALWAYS;
	}
	if( op == "nequal" )
	{
		return COMPARE_NOTEQUAL;
	}
	if( op == "less" )
	{
		return COMPARE_LESS;
	}
	if( op == "lequal" )
	{
		return COMPARE_LEQUAL;
	}
	if( op == "greater" )
	{
		return COMPARE_GREATER;
	}
	if( op == "gequal" )
	{
		return COMPARE_GEQUAL;
	}
	return COMPARE_LEQUAL;
}

//--------------------------------------------------------------------------
/**
* StringToCullMode
*/
eRasterCullMode Shader::StringToCullMode( std::string cullmode )
{
	if( cullmode == "front" )
	{
		return CULL_FRONT;
	}
	if( cullmode == "back" )
	{
		return CULL_BACK;
	}
	return CULL_NONE;
}
