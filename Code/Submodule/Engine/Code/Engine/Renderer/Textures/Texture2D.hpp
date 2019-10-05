#pragma once
#include "Engine/Renderer/Textures/Texture.hpp"
#include "Engine/Renderer/Textures/TextureView2D.hpp"

class Image;
class DepthStencilTargetView;
class ColorTargetView;

class Texture2D
	: public Texture
{
	friend class RenderContext;
	friend class TextureView2D;
	friend class Texture2D;

public:
	Texture2D( RenderContext* renderContext );
	~Texture2D();

public: // Helper statics
 	static Texture2D* CreateDepthStencilTarget( RenderContext *ctx, int widht, int height );
	static Texture2D* CreateDepthStencilTargetFor( Texture2D *colorTarget );
	static Texture2D* CreateColorTarget( RenderContext* ctx, uint width, uint height ); 
	static Texture2D* CreateMatchingColorTarget( Texture2D* other );

	Texture2D* CreateDepthStencilTarget( int widht, int height ); 
	DepthStencilTargetView* CreateDepthStencilTargetView() const;
	ColorTargetView* CreateColorTargetView() const;              

public:

	bool LoadFromFile( std::string const &filePath, bool flip = false );
	bool LoadFromImage( Image const &image );

	TextureView2D* CreateTextureView2D() const;  
	
private:
	void FreeHandles();

};
