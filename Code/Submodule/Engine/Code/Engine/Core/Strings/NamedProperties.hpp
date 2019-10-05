#pragma once
#include "Engine/Core/Strings/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XML/XMLUtils.hpp"
#include <string>
#include <map>



class PropertyBase 
{
public:
	virtual std::string AsString() const = 0; 
}; 

//--------------------------------------------------------------------------

template <typename OBJ_TYPE>
class TypedProperty : public PropertyBase
{
public:
	TypedProperty( OBJ_TYPE value );
	~TypedProperty();

public:
	virtual std::string AsString() const;         

public:
	OBJ_TYPE m_value;

}; 

//--------------------------------------------------------------------------
/**
* ToString
* NOTE: Make sure that the Class has a defined ToString( class ) function.
* Check StringsUtils for examples.
*/
template <typename OBJ_TYPE>
std::string TypedProperty<OBJ_TYPE>::AsString() const
{
	return ToString( m_value ); 
}

//--------------------------------------------------------------------------
/**
* TypedProperty
*/
template <typename OBJ_TYPE>
TypedProperty<OBJ_TYPE>::TypedProperty( OBJ_TYPE value )
{
	m_value = value;
}

//--------------------------------------------------------------------------
/**
* ~TypedProperty
*/
template <typename OBJ_TYPE>
TypedProperty<OBJ_TYPE>::~TypedProperty()
{

}

//--------------------------------------------------------------------------

class NamedProperties
{
public:
	NamedProperties();
	~NamedProperties(); 

	void PopulateFromXmlElementAttributes( const XmlElement& element );
	std::string GetPropertyString( std::string const &name, std::string const &def = "" ); 

public:
	// Template interface we want;
	template <typename OBJ_TYPE>
	void SetValue( const std::string& id, const OBJ_TYPE& value );

	template <typename OBJ_TYPE>
	OBJ_TYPE GetValue( const std::string& id, const OBJ_TYPE& defaultValue );

	void SetValue( const std::string& id, const char* var );

	std::string GetValue( const std::string& id, const char* defaulfValue );

	void operator=( const NamedProperties& copyFrom );	

private: 
	// supporting methods; 
	PropertyBase* GetProperty( const std::string& name ) const; 

public:
	std::map<std::string, PropertyBase*> m_properties; 

private:
	template <typename OBJ_TYPE>
	OBJ_TYPE FromString( const char* str, const OBJ_TYPE& def );

	float FromString( const char* str, const float& def )
	{
		(void)(def);
		return StringToFloat(str);
	}

	int FromString( const char* str, const int& def )
	{
		(void)(def);
		return StringToInt(str);
	}

	std::string FromString( const char* str, const std::string& def )
	{
		(void)(def);
		return str;
	}

	bool FromString( const char* str, const bool& def )
	{
		(void)(def);
		return str;
	}

}; 


template <typename OBJ_TYPE>
void NamedProperties::SetValue( const std::string& id, const OBJ_TYPE& value )
{
	if( m_properties.find( id ) != m_properties.end() )
	{
		delete m_properties[id];
		m_properties[id] = nullptr;
	}
	m_properties[id] = new TypedProperty<OBJ_TYPE>( value );
}



template <typename OBJ_TYPE>
OBJ_TYPE NamedProperties::GetValue( const std::string& id, const OBJ_TYPE& defaultValue ) 
{
	if( m_properties.find( id ) != m_properties.end() )
	{
		PropertyBase* prop = m_properties[id]; 
		TypedProperty<OBJ_TYPE>* valueAsT = dynamic_cast<TypedProperty<OBJ_TYPE>*>( prop );
		if( valueAsT )
		{
			return valueAsT->m_value;
		}

		TypedProperty<std::string>* strVal = dynamic_cast<TypedProperty<std::string>*>( prop );
		if( strVal )
		{
			return FromString( strVal->AsString().c_str(), defaultValue );
		}
		return defaultValue;
	}
	return defaultValue;
}



//--------------------------------------------------------------------------
template <typename OBJ_TYPE>
OBJ_TYPE NamedProperties::FromString( const char* str, OBJ_TYPE const &def )
{
	(void)(def);
	return OBJ_TYPE(str); 
}
//--------------------------------------------------------------------------