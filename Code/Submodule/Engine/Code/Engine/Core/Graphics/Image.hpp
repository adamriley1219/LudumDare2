#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

// NOT A TEXTURE, Don't use to draw with.
class Image
{
public:
	Image();
	explicit Image( const char* imageFilePath, bool flip = false );
	~Image();

	void LoadFromFile( const char* imageFilePath, bool flip = false );

	Rgba GetTexelColor( int texelX, int texelY ) const;
	Rgba GetTexelColor( const IntVec2& texelCoords ) const;
	const IntVec2&		GetDimensions() const;
	const std::string&	GetFilePath()	const;
	int GetWidth() const;
	int GetHeight() const;
	int GetBytesPerPixel() const;
	unsigned char* GetBuffer() const;
	unsigned int GetSize() const;

	void SetTexelColor( int texelX, int texelY, const Rgba& color );
	void SetTexelColor( const IntVec2& texelCoords, const Rgba& color );
	void SetFilepath( const std::string& filepath );

private:
	IntVec2					m_dimensions = IntVec2::ONE;
	std::vector< Rgba >		m_texels;
	unsigned char*			m_rawData;
	std::string				m_imageFilePath;
};

