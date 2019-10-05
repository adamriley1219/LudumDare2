#include "Engine/Core/XML/XMLUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"



//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
	const char* value = element.Attribute(attributeName);
	int returnValue = defaultValue;
	if( value )
	{
		returnValue = StringToInt( Stringf( value ) );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
	const char* value = element.Attribute(attributeName);
	char returnValue = defaultValue;
	if( value )
	{
		returnValue = value[0];		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
	const char* value = element.Attribute(attributeName);
	bool returnValue = defaultValue;
	if( value )
	{
		std::string val = Stringf(value);
		returnValue = val == "true";		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
	const char* value = element.Attribute(attributeName);
	float returnValue = defaultValue;
	if( value )
	{
		returnValue = StringToFloat( Stringf( value ) );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
Rgba ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	Rgba returnValue = defaultValue;
	if( value )
	{
		returnValue = Rgba( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXMLAttribute
*/
Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	Vec2 returnValue = defaultValue;
	if( value )
	{
		returnValue = Vec2( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	Vec3 returnValue = defaultValue;
	if( value )
	{
		returnValue = Vec3( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	IntRange returnValue = defaultValue;
	if( value )
	{
		returnValue = IntRange( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	FloatRange returnValue = defaultValue;
	if( value )
	{
		returnValue = FloatRange( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	IntVec2 returnValue = defaultValue;
	if( value )
	{
		returnValue = IntVec2( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* value = element.Attribute(attributeName);
	std::string returnValue = defaultValue;
	if( value )
	{
		returnValue = Stringf( value );		
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue/*=nullptr */ )
{
	const char* value = element.Attribute(attributeName);
	std::string returnValue = Stringf( defaultValue );
	if( value )
	{
		returnValue = Stringf( value );
	}
	return returnValue;
}

//--------------------------------------------------------------------------
/**
* ParseXmlAttribute
*/
uint ParseXmlAttribute(const XmlElement& element, const char* attributeName, uint defaultValue)
{
	const char* value = element.Attribute(attributeName);
	uint returnValue = defaultValue;
	if( value )
	{
		returnValue = StringToInt( Stringf( value ) );		
	}
	return returnValue;
}
