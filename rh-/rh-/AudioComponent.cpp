#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent()
{
	_mute = false;
	_playOnAwake = true;
	_loop = true;
}

AudioComponent::AudioComponent(std::string path)
{
	_path = path;
	_mute = false;
	_playOnAwake = true;
	_loop = true;
}


AudioComponent::~AudioComponent()
{
}

std::string AudioComponent::GetPath()
{
	return _path;
}

void AudioComponent::SetPath(std::string path)
{
	_path = path;
}

bool AudioComponent::GetMute()
{
	return _mute;
}

void AudioComponent::SetMute(bool muteState)
{
	_mute = muteState;
}

bool AudioComponent::GetPlayOnAwake()
{
	return _playOnAwake;
}

void AudioComponent::SetPlayOnAwake(bool playOnAwakeState)
{
	_playOnAwake = playOnAwakeState;
}

bool AudioComponent::GetLoop()
{
	return _loop;
}

void AudioComponent::SetLoop(bool loopState)
{
	_loop = loopState;
}

float AudioComponent::GetVolume()
{
	return _volume;
}

void AudioComponent::SetVolume(float volume)
{
	_volume = volume;
}

ComponentType AudioComponent::GetType(void)
{
	ComponentType componentType;
	componentType.name = "Audio";
	return componentType;
}
