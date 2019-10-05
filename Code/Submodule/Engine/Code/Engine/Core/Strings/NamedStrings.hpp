#pragma once
#include <map>
#include "Engine/Core/XML/XMLUtils.hpp"

struct Rgba;
struct IntVec2;
struct Vec2;
struct FloatRange;
struct IntRange;

#ifdef _WIN32
#define PLATFORM_WINDOWS
class Camera;
#endif

class NamedStrings
{
public:
	NamedStrings();
	~NamedStrings();

	void			PopulateFromXmlElementAttributes( const XmlElement& element );
	void			SetValue( const std::string& keyName, const std::string& newValue );

	bool			GetValue( const std::string& keyName, bool defaultValue ) const;
	int				GetValue( const std::string& keyName, int defaultValue ) const;
	float			GetValue( const std::string& keyName, float defaultValue ) const;
	std::string		GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string		GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba			GetValue( const std::string& keyName, const Rgba& defaultValue ) const;
	Vec2			GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	IntVec2			GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;
	FloatRange		GetValue( const std::string& keyName, const FloatRange& defaultValue ) const;
	IntRange		GetValue( const std::string& keyName, const IntRange& defaultValue ) const;

#ifdef PLATFORM_WINDOWS
	Camera			GetValue( const std::string& keyName, const Camera& defaultValue ) const;
#endif

private:
	std::map< std::string, std::string > m_keyValuePairs;
};

