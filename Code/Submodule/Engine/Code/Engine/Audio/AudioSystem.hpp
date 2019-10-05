#pragma once


//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Math/Vec3.hpp"

#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
class AudioSystem;
struct Vec3;

//--------------------------------------------------------------------------

enum eCLIPTYPE
{
	CLIPTYPE_2D,
	CLIPTYPE_3D,
};

//--------------------------------------------------------------------------

struct Clip
{
public:
	SoundID soundID = 0;
	std::string filePath = "NO_PATH";
	std::string mixer = "NO_MIXER";
	eCLIPTYPE type = CLIPTYPE_2D;
	bool stream = false;
	bool loop = false;

public:
	Clip(){}
	explicit Clip( const std::string& filePath, SoundID soundID, const std::string& mix = "master", bool stream = false, bool loop = false, eCLIPTYPE type = CLIPTYPE_2D );

};

struct Cue
{
public:
	std::vector<Clip*> clips;
	std::string id = "NO_ID";
	std::string mixer = "NO_PARENT";

public:
	Cue(){}
	explicit Cue( const std::string& id, const std::string& mixID );
	void AddClip( Clip* clip );
	Clip* GetRandomClip();

};

struct Mix
{
public:
	FMOD::ChannelGroup* group = nullptr;
	std::string id = "NO_ID";
	std::string parent = "NO_PARENT";

public:
	Mix(){}
	explicit Mix( const std::string& id, FMOD::ChannelGroup* channelGroup, const std::string& parent );

};

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

public:
	void LoadFromXML( const char* filePath );

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual Clip*				CreateOrGetClip( const std::string& soundFilePath, const std::string& mixer = "master", bool stream = false, bool loop = false, eCLIPTYPE type = CLIPTYPE_2D );
	virtual Cue*				CreateOrGetCue( const std::string& id, const std::string& mix = "master" );
	virtual Mix*				CreateOrGetMixer( const std::string& mixName, const std::string& parent = "master" );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual SoundPlaybackID		PlaySound( const Clip& clip , float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual SoundPlaybackID		Play3DSoundAt( const Clip& clip, const Vec3& position, const Vec3& vel = Vec3::ZERO, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	
	virtual void				SetListenerPos( const Vec3& position, uint listnerID = 0, const Vec3& facing = Vec3::FORWARD, const Vec3& up = Vec3::UP, const Vec3& vel = Vec3::ZERO );

	static void					StopSound( SoundPlaybackID soundPlaybackID );
	static void					SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	static void					SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	static void					SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::vector< FMOD::Sound* >			m_registeredSounds;

	std::map< std::string, Clip >		m_registeredClips;
	std::map< std::string, Cue >		m_registeredCues;
	std::map< std::string, Mix >		m_registeredMixes;
};

