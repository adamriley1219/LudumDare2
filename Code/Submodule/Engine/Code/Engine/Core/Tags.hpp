#pragma once
#include "Engine/Core/EngineCommon.hpp"


class Tags
{
public:
	// These support comma-separated strings of 0+ tag names, with “!” (NOT) tag names to remove
	void SetOrRemoveTags( const std::string& commaSeparatedTagNames ); // "blessed,!poisoned, ..."
	bool HasTags( const std::string& commaSeparatedTagNames ); // "good,!cursed, ..."

	// These each take exactly one tag name, and do not support commas nor “!” (NOT) tags
	void SetTag( const std::string& tagName );
	void RemoveTag( const std::string& tagName );
	bool HasTag( const std::string& tagName );

protected:
	Strings m_tags;

};

