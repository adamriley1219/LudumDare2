#pragma once




struct Rgba
{
public:
	float r;
	float g;
	float b;
	float a;

public:
	const static Rgba WHITE;
	const static Rgba BLACK;
	const static Rgba RED;
	const static Rgba DARK_RED;
	const static Rgba LIGHT_RED;
	const static Rgba YELLOW;
	const static Rgba GREEN;
	const static Rgba DARK_GREEN;
	const static Rgba BLUE;
	const static Rgba LIGHT_BLUE;
	const static Rgba GRAY;
	const static Rgba DARK_GRAY;
	const static Rgba CYAN;
	const static Rgba MAGENTA;
	const static Rgba FADED_GRAY;
	const static Rgba FADED_DARK_GRAY;
	const static Rgba FADED_BLACK;
	const static Rgba INVISABLE;

public:
	Rgba();
	~Rgba() {}
	explicit Rgba( const char* text );
	explicit Rgba(float red, float green, float blue, float alpha = 1.f);
	explicit Rgba(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

	void operator=( const Rgba& copyFrom );		
	bool operator==( const Rgba& compare ) const;				
	const Rgba operator*( const float uniformScale ) const;				

	void SetFromText( const char* text );
};