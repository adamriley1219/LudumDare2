#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

//--------------------------------------------------------------------------
/**
* SpriteAnimDefinition
*/
SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, eSpriteAnimPlaybackMode playbackType/*=SPRITE_ANIM_PLAYBACK_LOOP */ )
	: m_spriteSheet(sheet), m_startSpriteIndex(startSpriteIndex), m_endSpriteIndex(endSpriteIndex), m_durationSeconds(durationSeconds),  m_playbackType(playbackType)
{

}

//--------------------------------------------------------------------------
/**
* GetSpriteDefAtTime
*/
const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int numFrames = m_endSpriteIndex - m_startSpriteIndex;
	int numBaseFrames = numFrames + 1;
	float fracComplete = seconds / m_durationSeconds;

	switch( m_playbackType )
	{
	case SPRITE_ANIM_PLAYBACK_ONCE:
		{	
			int animFrameNumber = ( (int) floorf( fracComplete  * (float) numBaseFrames ) );
			if( animFrameNumber > m_endSpriteIndex - m_startSpriteIndex )
				animFrameNumber = numFrames;

			return m_spriteSheet.GetSpriteDefinition( animFrameNumber + m_startSpriteIndex );
		}
	case SPRITE_ANIM_PLAYBACK_LOOP:
		{
			int animFrameNumber = ( (int) floorf( fracComplete  * (float) numBaseFrames ) );
			animFrameNumber = animFrameNumber % numBaseFrames;
			return m_spriteSheet.GetSpriteDefinition( animFrameNumber + m_startSpriteIndex );
		}
	case SPRITE_ANIM_PLAYBACK_PINGPONG:
		{
			int numFramesInCycle = ( m_endSpriteIndex - m_startSpriteIndex ) * 2;

			int animFrameNumber =( (int) floorf( fracComplete  * (float) numFramesInCycle ) );

			int curFrameInCycle = animFrameNumber % numFramesInCycle;
			if( curFrameInCycle < numBaseFrames )
			{
				return m_spriteSheet.GetSpriteDefinition( m_startSpriteIndex + curFrameInCycle );
			}
			int indexIntoPongback = ( numFramesInCycle ) - curFrameInCycle;
			return m_spriteSheet.GetSpriteDefinition( indexIntoPongback + m_startSpriteIndex );
		}
	}
	return m_spriteSheet.GetSpriteDefinition(0);
}

//--------------------------------------------------------------------------
/**
* GetTextureView
*/
const TextureView* SpriteAnimDefinition::GetTextureView() const
{
	return m_spriteSheet.GetTextureView();
}

//--------------------------------------------------------------------------
/**
* AddEvent
*/
void SpriteAnimDefinition::AddEvent( float time, const std::string& action )
{
	m_events.push_back( AnimEvent( time, action ) );
}

//--------------------------------------------------------------------------
/**
* GetEventsBetween
*/
std::vector<std::string> SpriteAnimDefinition::GetEventsBetween(float start, float end)
{
	std::vector<std::string> actions;
	if( m_durationSeconds <= 0.0f )
	{
		return actions;
	}

	float loopStart = start;
	float loopEnd = end;
	while ( loopStart > m_durationSeconds )
	{
		loopStart -= m_durationSeconds;
	}
	while ( loopEnd > m_durationSeconds )
	{
		loopEnd -= m_durationSeconds;
	}

	for( AnimEvent event : m_events )
	{
		switch ( m_playbackType )
		{
		case SPRITE_ANIM_PLAYBACK_ONCE:
			if( start <= event.m_fireTime && event.m_fireTime < end )
			{
				actions.push_back( event.m_action );
			}
			break;
		case SPRITE_ANIM_PLAYBACK_LOOP:
			if( ( loopStart <= event.m_fireTime && event.m_fireTime < loopEnd ) || ( loopEnd < loopStart &&  loopStart <= event.m_fireTime ) )
			{
				actions.push_back( event.m_action );
			}
			break;
		case SPRITE_ANIM_PLAYBACK_PINGPONG:
			//TODO
			break;
		default:
			break;
		}
	}
	return actions;
}

//--------------------------------------------------------------------------
/**
* GetTimeFromFrame
* Assumes in Play Once Mode
*/
float SpriteAnimDefinition::GetTimeFromFrame( int frameNum ) const
{
	return (  ( (float) frameNum / (float) GetFrameCount() ) * GetDuration() );
}
