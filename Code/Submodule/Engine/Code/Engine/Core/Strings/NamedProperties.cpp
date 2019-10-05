#include "Engine/Core/Strings/NamedProperties.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------
/**
* NamedProperties
*/
NamedProperties::NamedProperties()
{

}

//--------------------------------------------------------------------------
/**
* ~NamedProperties
*/
NamedProperties::~NamedProperties()
{
	for( auto p : m_properties )
	{
		SAFE_DELETE( p.second );
	}
}

//--------------------------------------------------------------------------
/**
* PopulateFromXmlElementAttributes
*/
void NamedProperties::PopulateFromXmlElementAttributes( const XmlElement& element )
{
	for( const tinyxml2::XMLAttribute* att = element.FirstAttribute(); att; att = att->Next() )
	{
		SetValue( att->Name(), att->Value() );
	}
}

//--------------------------------------------------------------------------
/**
* GetPropertyString
*/
std::string NamedProperties::GetPropertyString( std::string const &name, std::string const &def /*= "" */ )
{
	PropertyBase* base = GetProperty( name );
	if( base )
	{
		return base->AsString();
	}
	return def;
}

//--------------------------------------------------------------------------
/**
* Set
*/
void NamedProperties::SetValue( std::string const &id, char const *var )
{
	SetValue<std::string>( id, std::string( var ) );
}

//--------------------------------------------------------------------------
/**
* Get
*/
std::string NamedProperties::GetValue( std::string const &id, char const *defaulfValue )
{
	return GetValue<std::string>( id, std::string( defaulfValue ) );
}

//--------------------------------------------------------------------------
/**
* operator=
*/
void NamedProperties::operator=( const NamedProperties& copyFrom )
{
	m_properties = copyFrom.m_properties;
}

//--------------------------------------------------------------------------
/**
* GetProperty
*/
PropertyBase* NamedProperties::GetProperty( std::string const &name ) const
{
	auto itr = m_properties.find( name );
	if( itr != m_properties.end() )
	{
		return itr->second;
	}
	return nullptr;
}
