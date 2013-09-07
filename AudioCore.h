#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <Windows.h>
#include <stdio.h>
#include "AudioCoreEnums.h"

class AudioCore
{
private:
	FMOD::System* system;
	FMOD_RESULT result;
	unsigned int version;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	char name[256];
	FMOD::Channel* m_SoundEffectChannelOne;
	FMOD::Channel* m_SoundEffectChannelTwo;
	FMOD::Channel* m_SoundEffectChannelThree;
	FMOD::Channel* m_StreamChannelOne;
	FMOD::Channel* m_StreamChannelTwo;
	FMOD::Channel* m_StreamChannelThree;
	float SoundOneVol;
	float SoundTwoVol;
	float SoundThreeVol;
	float StreamOneVol;
	float StreamTwoVol;
	float StreamThreeVol;
	FMOD::Sound* m_SoundOne;
	FMOD::Sound* m_SoundTwo;
	FMOD::Sound* m_SoundThree;
	FMOD::Sound* m_StreamOne;
	FMOD::Sound* m_StreamTwo;
	FMOD::Sound* m_StreamThree;
public:
	AudioCore();
	~AudioCore();
	void LoadStream(const char *AudioFileName,int StreamID);
	void LoadSound(const char* AudioFileName,int SoundID);
	void PlayStream(int StreamID);
	void PlaySoundFile(int SoundID);
	void Update();
	void SetStreamVolume(float vol);
	void SetStreamVolumeByID(float vol,int StreamID);
	void SetSoundVolume(float vol);
	void SetSoundVolumeByID(float vol,int SoundID);
	void ReleaseSoundByID(int SoundID);
	void ReleaseStreamByID(int StreamID);
};