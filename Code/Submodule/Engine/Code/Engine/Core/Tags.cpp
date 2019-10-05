#include "Engine/Core/Tags.hpp"



//--------------------------------------------------------------------------
/**
* SetOrRemoveTags
*/
void Tags::SetOrRemoveTags( const std::string& commaSeparatedTagNames )
{
	if( commaSeparatedTagNames == "" )
	{
		return;
	}

	Strings splitTags = SplitStringOnDelitmiter( commaSeparatedTagNames.c_str(), "," );
	for( int tagIndex = 0; tagIndex < (int) splitTags.size(); tagIndex++ )
	{
		std::string tag = splitTags[tagIndex];
		if( tag[0] == '!' )
		{
			// Remove '!' from the tag
			std::string tagToRemove( tag, 1, ( (int) tag.size() ) - 2 );
			RemoveTag( tagToRemove );
		}
		else
		{
			SetTag( tag );
		}
	}
}

//--------------------------------------------------------------------------
/**
* HasTags
*/
bool Tags::HasTags( const std::string& commaSeparatedTagNames )
{
	if( commaSeparatedTagNames == "" )
	{
		return true;
	}

	Strings splitTags = SplitStringOnDelitmiter( commaSeparatedTagNames.c_str(), "," );
	bool hasTheTags = true;

	for( int tagIndex = 0; tagIndex < (int) splitTags.size(); tagIndex++ )
	{
		std::string tag = splitTags[tagIndex];
		if( tag[0] == '!' )
		{
			// Remove '!' from the tag
			std::string tagNotThere( tag, 1, ( (int) tag.size() ) - 2 );
			bool tagExists = HasTag( tagNotThere );
			if( tagExists )
			{
				hasTheTags = false;
			}
		}
		else
		{
			bool tagExists = HasTag( tag );
			if( !tagExists )
			{
				hasTheTags = false;
			}
		}
	}
	
	return hasTheTags;
}

//--------------------------------------------------------------------------
/**
* SetTag
*/
void Tags::SetTag( const std::string& tagName )
{
	GUARANTEE_OR_DIE( tagName != "", "Tags::SetTag was given an empty string." );
	for( int tagIndex = 0; tagIndex < (int) m_tags.size(); tagIndex++ )
	{
		std::string& tag = m_tags[tagIndex];
		if( tagName == tag )
		{
			return;
		}
	}
	std::string pushThis = tagName;
	m_tags.push_back( pushThis );
}

//--------------------------------------------------------------------------
/**
* RemoveTag
*/
void Tags::RemoveTag( const std::string& tagName )
{
	int tagsSize =  (int) m_tags.size();
	for( int tagIndex = 0; tagIndex < tagsSize; tagIndex++ )
	{
		std::string& tag = m_tags[tagIndex];
		if( tagName.compare( tag ) == 0 )
		{
			std::string endTag = m_tags[tagsSize];
			tag = endTag;
			m_tags.pop_back();
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* HasTag
*/
bool Tags::HasTag( const std::string& tagName )
{
	int tagsSize =  (int) m_tags.size();
	for( int tagIndex = 0; tagIndex < tagsSize; tagIndex++ )
	{
		std::string& tag = m_tags[tagIndex];
		if( tagName == tag )
		{
			return true;
		}
	}
	return false;
}
