#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Strings/NamedStrings.hpp"
#include "Engine/Core/Graphics/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Renderer/Camera.hpp"



//--------------------------------------------------------------------------
/**
* NamedStrings
*/
NamedStrings::NamedStrings()
{

}

//--------------------------------------------------------------------------
/**
* ~NamedStrings
*/
NamedStrings::~NamedStrings()
{

}

//--------------------------------------------------------------------------
/**
* PopulateFromXmlElementAttributes
*/
void NamedStrings::PopulateFromXmlElementAttributes( const XmlElement& element )
{
	for( const tinyxml2::XMLAttribute* att = element.FirstAttribute(); att; att = att->Next() )
	{
		SetValue( att->Name(), att->Value() );
	}
}

//--------------------------------------------------------------------------
/**
* SetValue
*/
void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else
	{
		return defaultValue;
	}
	return value == "true";
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return StringToInt( value );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return StringToFloat( value );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return value;
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return Stringf( defaultValue );
	}
	return value;
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
Rgba NamedStrings::GetValue( const std::string& keyName, const Rgba& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return Rgba( value.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return Vec2( value.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return IntVec2( value.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
FloatRange NamedStrings::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return FloatRange( value.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
IntRange NamedStrings::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return IntRange( value.c_str() );
}

//--------------------------------------------------------------------------
/**
* GetValue
*/
#ifdef PLATFORM_WINDOWS
Camera NamedStrings::GetValue( const std::string& keyName, const Camera& defaultValue ) const
{
	auto itr = m_keyValuePairs.find( keyName );
	std::string value;
	if( itr != m_keyValuePairs.end() )
	{
		value = itr->second;
	}
	else	
	{
		return defaultValue;
	}
	return Camera( value.c_str() );
}
#endif
