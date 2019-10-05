#pragma once
#include "Engine/Renderer/RenderUtil.hpp"
#include "Engine/Math/IntVec2.hpp"

struct ID3D11Resource;
struct ID3D11DepthStencilView;
class RenderContext;


class DepthStencilTargetView                           
{
public: 
	DepthStencilTargetView();                          
	~DepthStencilTargetView();                        

	int GetWidth() const;                           
	int GetHeight() const;                          

	void ClearDepth( RenderContext* context, float depth );

public: 
	ID3D11Resource *m_source         = nullptr;        
	ID3D11DepthStencilView *m_dsv    = nullptr;      
	IntVec2 m_size                   = IntVec2::ZERO; 
}; 
