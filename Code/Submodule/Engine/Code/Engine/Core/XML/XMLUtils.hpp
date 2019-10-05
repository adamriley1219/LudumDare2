#pragma once
#include <string>
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/EngineCommon.hpp"

struct Vec2;
struct Vec3;
struct IntVec2;
struct Rgba;
struct IntRange;
struct FloatRange;

typedef tinyxml2::XMLElement XmlElement;
typedef unsigned int uint;

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue );
uint ParseXmlAttribute( const XmlElement& element, const char* attributeName, uint defaultValue );
char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue );
Rgba ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba& defaultValue );
Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue );
Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue );
IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue );
IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue=nullptr );