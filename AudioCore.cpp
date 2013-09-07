#include "AudioCore.h"

void ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		MessageBox(HWND_DESKTOP, FMOD_ErrorString(result), "Error", MB_OK | MB_ICONEXCLAMATION);
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-5);
	}
}

AudioCore::AudioCore()
{
	ERRCHECK(FMOD::System_Create(&system));
	ERRCHECK(system->getVersion(&version));
	if (version < FMOD_VERSION)
	{
		//PostQuitMessage(10);
	}
	ERRCHECK(system->getNumDrivers(&numDrivers));
	if (numDrivers == 0)
	{
		ERRCHECK(system->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
	else
	{
		ERRCHECK(system->getDriverCaps(0, &caps, 0, &speakerMode));
		//Set the user selected speaker mode.
		ERRCHECK(system->setSpeakerMode(speakerMode));
		if (caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad
			for latency! You might want to warn the user about this.
			*/
			ERRCHECK(system->setDSPBufferSize(1024, 10));
		}
		result = system->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);
		if (strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			ERRCHECK(system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR));
		}
	}
	ERRCHECK(system->init(32, FMOD_INIT_NORMAL, 0));
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
		Ok, the speaker mode selected isn't supported by this soundcard. Switch it
		back to stereo...
		*/
		ERRCHECK(system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
		//... and re-init.
		ERRCHECK(system->init(32, FMOD_INIT_NORMAL, 0));
	}
	m_SoundEffectChannelOne=NULL;
	m_SoundEffectChannelTwo=NULL;
	m_SoundEffectChannelThree=NULL;
	m_StreamChannelOne=NULL;
	m_StreamChannelTwo=NULL;
	m_StreamChannelThree=NULL;
	m_SoundOne=NULL;
	m_SoundTwo=NULL;
	m_SoundThree=NULL;
	m_StreamOne=NULL;
	m_StreamTwo=NULL;
	m_StreamThree=NULL;
	SoundOneVol=1.0f;
	SoundTwoVol=1.0f;
	SoundThreeVol=1.0f;
	StreamOneVol=1.0f;
	StreamTwoVol=1.0f;
	StreamThreeVol=1.0f;
}

AudioCore::~AudioCore()
{
	system->release();
}

void AudioCore::LoadStream(const char *AudioFileName,int StreamID)
{
	switch (StreamID)
	{
	case StreamOne:
		{
			ERRCHECK(system->createStream(AudioFileName,FMOD_HARDWARE | FMOD_LOOP_NORMAL,0,&m_StreamOne));
			break;
		}
	case StreamTwo:
		{
			ERRCHECK(system->createStream(AudioFileName,FMOD_HARDWARE | FMOD_LOOP_NORMAL,0,&m_StreamTwo));
			break;
		}
	case StreamThree:
		{
			ERRCHECK(system->createStream(AudioFileName,FMOD_HARDWARE | FMOD_LOOP_NORMAL,0,&m_StreamThree));
			break;
		}
	default:
		break;
	}
}
void AudioCore::LoadSound(const char* AudioFileName,int SoundID)
{
	switch (SoundID)
	{
	case SoundEffectOne:
		{
			ERRCHECK(system->createSound(AudioFileName,FMOD_HARDWARE,0,&m_SoundOne));
			break;
		}
	case SoundEffectTwo:
		{
			ERRCHECK(system->createSound(AudioFileName,FMOD_HARDWARE,0,&m_SoundTwo));
			break;
		}
	case SoundEffectThree:
		{
			ERRCHECK(system->createSound(AudioFileName,FMOD_HARDWARE,0,&m_SoundThree));
			break;
		}
	default:
		break;
	}
}
void AudioCore::PlayStream(int StreamID)
{
	switch (StreamID)
	{
	case StreamOne:
		{
			m_StreamChannelOne->setVolume(StreamOneVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_StreamOne,false,&m_StreamChannelOne));
			break;
		}
	case StreamTwo:
		{
			m_StreamChannelTwo->setVolume(StreamTwoVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_StreamTwo,false,&m_StreamChannelTwo));
			break;
		}
	case StreamThree:
		{
			m_StreamChannelThree->setVolume(StreamThreeVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_StreamThree,false,&m_StreamChannelThree));
			break;
		}
	default:
		break;
	}
}
void AudioCore::PlaySoundFile(int SoundID)
{
	switch (SoundID)
	{
	case SoundEffectOne:
		{
			m_SoundEffectChannelOne->setVolume(SoundOneVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_SoundOne,false,&m_SoundEffectChannelOne));
			break;
		}
	case SoundEffectTwo:
		{
			m_SoundEffectChannelTwo->setVolume(SoundTwoVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_SoundTwo,false,&m_SoundEffectChannelTwo));
			break;
		}
	case SoundEffectThree:
		{
			m_SoundEffectChannelThree->setVolume(SoundThreeVol);
			ERRCHECK(system->playSound(FMOD_CHANNEL_FREE,m_SoundThree,false,&m_SoundEffectChannelThree));
			break;
		}
	default:
		break;
	}
}
void AudioCore::Update()
{
	m_StreamChannelOne->setVolume(StreamOneVol);
	m_StreamChannelTwo->setVolume(StreamTwoVol);
	m_StreamChannelThree->setVolume(StreamThreeVol);
	m_SoundEffectChannelOne->setVolume(SoundOneVol);
	m_SoundEffectChannelTwo->setVolume(SoundTwoVol);
	m_SoundEffectChannelThree->setVolume(SoundThreeVol);
	system->update();
}
void AudioCore::SetStreamVolume(float vol)
{
	StreamOneVol=vol;
	StreamTwoVol=vol;
	StreamThreeVol=vol;
}
void AudioCore::SetStreamVolumeByID(float vol,int StreamID)
{
	switch (StreamID)
	{
	case StreamOne:
		{
			StreamOneVol=vol;
			break;
		}
	case StreamTwo:
		{
			StreamTwoVol=vol;
			break;
		}
	case StreamThree:
		{
			StreamThreeVol=vol;
			break;
		}
	default:
		break;
	}
}
void AudioCore::SetSoundVolume(float vol)
{
	SoundOneVol=vol;
	SoundTwoVol=vol;
	SoundThreeVol=vol;
}
void AudioCore::SetSoundVolumeByID(float vol,int SoundID)
{
	switch (SoundID)
	{
	case SoundEffectOne:
		{
			SoundOneVol=vol;
			break;
		}
	case SoundEffectTwo:
		{
			SoundTwoVol=vol;
			break;
		}
	case SoundEffectThree:
		{
			SoundThreeVol=vol;
			break;
		}
	default:
		break;
	}
}
void AudioCore::ReleaseSoundByID(int SoundID)
{
	switch (SoundID)
	{
	case SoundEffectOne:
		{
			m_SoundOne->release();
			break;
		}
	case SoundEffectTwo:
		{
			m_SoundTwo->release();
			break;
		}
	case SoundEffectThree:
		{
			m_SoundThree->release();
			break;
		}
	default:
		break;
	}
}

void AudioCore::ReleaseStreamByID(int StreamID)
{
	switch (StreamID)
	{
	case StreamOne:
		{
			m_StreamOne->release();
			break;
		}
	case StreamTwo:
		{
			m_StreamTwo->release();
			break;
		}
	case StreamThree:
		{
			m_StreamThree->release();
			break;
		}
	default:
		break;
	}
}