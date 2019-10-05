#pragma once


struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

//--------------------------------------------------------------------------
class ShaderStage
{
	friend class Shader;
public:

	ShaderStage(){}
	~ShaderStage();

public:
	union {
		ID3D11Resource *m_handle; 
		ID3D11VertexShader *m_vertexShader; 
		ID3D11PixelShader *m_pixelShader; 
	};

};
