#pragma once
#include "Engine/Math/Vec2.hpp"

#include <vector>
#include <string>

class SpriteDefinition;
class SpriteSheet;
class TextureView;

//------------------------------------------------------------------------------------------------
enum eSpriteAnimPlaybackMode
{
	SPRITE_ANIM_PLAYBACK_ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4...
	SPRITE_ANIM_PLAYBACK_LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4...
	SPRITE_ANIM_PLAYBACK_PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2...
	NUM_SPRITE_ANIM_PLAYBACK_TYPES
};

struct AnimEvent
{
public:
	std::string m_action;
	float m_fireTime;

public:
	AnimEvent( float fireTime, const std::string& action ) : m_action(action), m_fireTime( fireTime ) {};

};

//------------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:
	explicit SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex,
		float durationSeconds, eSpriteAnimPlaybackMode playbackType=SPRITE_ANIM_PLAYBACK_LOOP );

	const SpriteDefinition& GetSpriteDefAtTime( float seconds ) const;
	const TextureView*			GetTextureView() const;
	void SetPlayMode( eSpriteAnimPlaybackMode mode ) { m_playbackType = mode; }

	void AddEvent( float time, const std::string& action );
	std::vector<std::string> GetEventsBetween( float start, float end );

	float GetDuration() const { return m_durationSeconds; }
	int GetFrameCount() const { return m_endSpriteIndex - m_startSpriteIndex + 1; }
	float GetTimeFromFrame( int frameNum ) const;

private:
	const SpriteSheet&		m_spriteSheet;
	int						m_startSpriteIndex	= -1;
	int						m_endSpriteIndex	= -1;
	float					m_durationSeconds	= 1.0f;
	eSpriteAnimPlaybackMode	m_playbackType		= SPRITE_ANIM_PLAYBACK_LOOP;
	std::vector<AnimEvent>	m_events;
};
