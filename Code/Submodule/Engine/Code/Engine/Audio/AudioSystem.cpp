#include "Engine/Audio/AudioSystem.hpp"


#include "Engine/Core/XML/XMLUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Strings/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )

#if defined( _WIN32 )

//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{


	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

// 	FMOD_RESULT driverRes = m_fmodSystem->setDriver(0);
// 	ValidateResult( driverRes );
	result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );


	FMOD::ChannelGroup* group = nullptr;
	ValidateResult( m_fmodSystem->createChannelGroup( "master", &group ) );
	m_registeredMixes[ "master" ] = Mix( "master", group, "MASTER_HAS_NO_PARENT" );
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//--------------------------------------------------------------------------
/**
* LoadFromXML
*/
void AudioSystem::LoadFromXML( const char* filePath )
{
	tinyxml2::XMLDocument config;
	config.LoadFile(filePath);
	XmlElement* root = config.RootElement();

	if( root )
	{
		for( XmlElement* audioEle = root->FirstChildElement( "mix" ); audioEle; audioEle = audioEle->NextSiblingElement( "mix" ) )
		{
			std::string id = ParseXmlAttribute( *audioEle,  "id", "NO_ID" );
			std::string parent = ParseXmlAttribute( *audioEle,  "parent", "NO_PARENT" );

			CreateOrGetMixer( id, parent );
		}
		for( XmlElement* audioEle = root->FirstChildElement( "clip" ); audioEle; audioEle = audioEle->NextSiblingElement( "clip" ) )
		{
			bool stream = ParseXmlAttribute( *audioEle,  "stream", false );
			bool loop = ParseXmlAttribute( *audioEle,  "loop", false );
			std::string type = ParseXmlAttribute( *audioEle,  "type", "2d" );
			std::string src = ParseXmlAttribute( *audioEle,  "src", "NO_SOURCE" );

			CreateOrGetClip( src, "master", stream, loop, type == "3d" ? CLIPTYPE_3D : CLIPTYPE_2D );
		}
		for( XmlElement* audioEle = root->FirstChildElement( "cue" ); audioEle; audioEle = audioEle->NextSiblingElement( "cue" ) )
		{
			std::string id = ParseXmlAttribute( *audioEle,  "id", "NO_ID" );
			std::string mix = ParseXmlAttribute( *audioEle,  "mix", "NO_MIX" );

			Cue* cue = CreateOrGetCue( id, mix );


			for( XmlElement* clipEle = audioEle->FirstChildElement( "clip" ); clipEle; clipEle = clipEle->NextSiblingElement( "clip" ) )
			{
				bool stream = ParseXmlAttribute( *clipEle,  "stream", false );
				bool loop = ParseXmlAttribute( *clipEle,  "loop", false );
				std::string type = ParseXmlAttribute( *clipEle,  "type", "2d" );
				std::string src = ParseXmlAttribute( *clipEle,  "src", "NO_SOURCE" );

				cue->AddClip( CreateOrGetClip( src, mix, stream, loop, type == "3d" ? CLIPTYPE_3D : CLIPTYPE_2D ) );
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//--------------------------------------------------------------------------
/**
* CreateOrGetClip
*/
Clip* AudioSystem::CreateOrGetClip(const std::string& soundFilePath, const std::string& mixer /*= "master"*/, bool stream /*= false*/, bool loop /*= false*/, eCLIPTYPE type /*= CLIPTYPE_2D */)
{
	std::map< std::string, Clip >::iterator found = m_registeredClips.find( soundFilePath );
	if( found != m_registeredClips.end() )
	{
		return &found->second;
	}
	else
	{
		uint options = 0U; 
		if ( type == CLIPTYPE_3D ) 
		{
			options |= FMOD_3D; // if you want to use it as 3D - you want to load it this way,
								// though technically you can change after load, but the documentaiton
								// recommends against it; 
		}
		if ( loop ) 
		{
			options |= FMOD_LOOP_NORMAL;  
		}
		if ( stream ) 
		{
			options |= FMOD_CREATESTREAM; 
		}

		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), options, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredClips[ soundFilePath ] = Clip( soundFilePath, newSoundID, mixer, stream, loop, type );
			m_registeredSounds.push_back( newSound );
			return &m_registeredClips[ soundFilePath ];
		}
	}

	ASSERT_RECOVERABLE( 0, "Sound failed to load." );
	return nullptr;
}

//--------------------------------------------------------------------------
/**
* CreateOrGetCue
*/
Cue* AudioSystem::CreateOrGetCue(const std::string& id, const std::string& mix /*= "master" */)
{
	std::map< std::string, Cue >::iterator found = m_registeredCues.find( id );
	if( found != m_registeredCues.end() )
	{
		return &found->second;
	}
	else
	{
		m_registeredCues[ id ] = Cue( id, mix );
		return &m_registeredCues[ id ];
	}

// 	ASSERT_RECOVERABLE( 0, "Sould never get here..." );
// 	return nullptr;
}

//--------------------------------------------------------------------------
/**
* CreateOrGetMixer
*/
Mix* AudioSystem::CreateOrGetMixer(const std::string& mixName, const std::string& parent /*= "master" */)
{
	std::map< std::string, Mix >::iterator found = m_registeredMixes.find( mixName );
	if( found != m_registeredMixes.end() )
	{
		return &found->second;
	}
	else
	{
		FMOD::ChannelGroup* group = nullptr;
		ValidateResult( m_fmodSystem->createChannelGroup( mixName.c_str(), &group ) );
		m_registeredMixes[ mixName ] = Mix( mixName, group, parent );

		std::map< std::string, Mix >::iterator foundItr = m_registeredMixes.find( parent );
		ASSERT_OR_DIE( foundItr != m_registeredMixes.end(), "Trying to parent a mixer to a non-existing mixer." );
		foundItr->second.group->addGroup( group );

		return &m_registeredMixes[ mixName ];
	}

// 	ASSERT_RECOVERABLE( 0, "Sould never get here..." );
// 	return nullptr;
}

//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}


//--------------------------------------------------------------------------
/**
* PlaySound
*/
SoundPlaybackID AudioSystem::PlaySound( const Clip& clip, float volume/*=1.f*/, float balance/*=0.0f*/, float speed/*=1.0f*/, bool isPaused/*=false */ )
{
	size_t numSounds = m_registeredSounds.size();
	if( clip.soundID < 0 || clip.soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ clip.soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	Mix* mixer = CreateOrGetMixer( clip.mixer );

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, mixer ? mixer->group : nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = clip.loop ? -1 : 0;
		unsigned int playbackMode = clip.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;}

//--------------------------------------------------------------------------
/**
* Play3DSoundAt
*/
SoundPlaybackID AudioSystem::Play3DSoundAt(const Clip& clip, const Vec3& position, const Vec3& vel /*= Vec3::ZERO*/, float volume/*=1.f*/, float balance/*=0.0f*/, float speed/*=1.0f*/, bool isPaused/*=false */)
{
	// playing a sound (code you already have) - started paused
	FMOD::Sound* sound = m_registeredSounds[ clip.soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	Mix* mixer = CreateOrGetMixer( clip.mixer );

	FMOD::Channel *channel = nullptr; 
	SoundPlaybackID playback = m_fmodSystem->playSound( sound, mixer ? mixer->group : nullptr /*mixer*/, true /*paused*/, &channel ); 

	if( channel )
	{
		int loopCount = clip.loop ? -1 : 0;
		unsigned int playbackMode = clip.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channel->setMode(playbackMode);
		channel->getFrequency( &frequency );
		channel->setFrequency( frequency * speed );
		channel->setVolume( volume );
		channel->setPan( balance );
		channel->setLoopCount( loopCount );
	}

	// the below only work IF the sound is set to be a 3D sound; 

	// now, given a channel, we can set the position of the audio
	// vec3 pos; // used for positional audio - set to world position of sound
	// vec3 vel; // used for doppler effects - feel free to default to 0 for now; 
	channel->set3DAttributes( (FMOD_VECTOR*)&position, (FMOD_VECTOR*)&vel ); 

		// set falloff rules.  By default, they use an inverse falloff,
		// which you can think of for each doubling of min distance, you half the 
		// volume.  Attenuation stops at max distance (sound will not get quieter)
		channel->setMode( FMOD_3D_INVERSEROLLOFF ); // default, you don't need to call this; 
													// see docs for more options (FMOD_3D_LINEARROLLOFF is particulary useful for games)
	channel->set3DMinMaxDistance( 1.0f, 10000.0f ); // default values - how they are used depends on the mode

													// resume the sound now that it is setup; 
	channel->setPaused( isPaused );
	return playback;
}

//--------------------------------------------------------------------------
/**
* SetListenerPos
*/
void AudioSystem::SetListenerPos( const Vec3& position, uint listnerID /*= 0*/, const Vec3& facing /*= Vec3::FORWARD*/, const Vec3& up /*= Vec3::UP*/, const Vec3& vel /*= Vec3::ZERO */)
{
	m_fmodSystem->set3DListenerAttributes( listnerID, (FMOD_VECTOR*)&position, (FMOD_VECTOR*)&vel, (FMOD_VECTOR*)&facing, (FMOD_VECTOR*)&up );
}

//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}



//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}


//--------------------------------------------------------------------------
/**
* Mix
*/
Mix::Mix(const std::string& ID, FMOD::ChannelGroup* channelGroup, const std::string& theParent)
	: group( channelGroup ), id( ID ), parent( theParent )
{

}

//--------------------------------------------------------------------------
/**
* Clip
*/
Clip::Clip(const std::string& path, SoundID theSoundID, const std::string& mix /*= "master"*/, bool streaming /*= false*/, bool looping /*= false*/, eCLIPTYPE theType /*= CLIPTYPE_2D */)
	: filePath( path ), soundID( theSoundID ), mixer( mix ), stream( streaming ), loop( looping ), type( theType )
{

}

//--------------------------------------------------------------------------
/**
* Cue
*/
Cue::Cue(const std::string& ID, const std::string& mixID)
	: id( ID ), mixer( mixID )
{

}

//--------------------------------------------------------------------------
/**
* AddClip
*/
void Cue::AddClip(Clip* clip)
{
	clips.push_back( clip );
}

//--------------------------------------------------------------------------
/**
* GetRandomClip
*/
Clip* Cue::GetRandomClip()
{
	if( clips.empty() )
	{
		return nullptr;
	}
	uint randIdx = GetRandomUINTInRange( 0, (uint) clips.size() - 1 );
	return clips[randIdx];
}
#else

// TODO: Implement other Audio system for non windows os.

#endif // end define of Windows


#endif // !defined( ENGINE_DISABLE_AUDIO )




