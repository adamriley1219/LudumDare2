#pragma once
#include "Engine/Renderer/Textures/TextureView.hpp"
#include "Engine/Math/IntVec2.hpp"

class Image;

class TextureView2D 
	: public TextureView
{
	friend class Texture2D;
	friend class RenderContext;

public:
	TextureView2D();
	~TextureView2D();

private:
	IntVec2 m_dimensions;

};
