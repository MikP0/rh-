#include "pch.h"
#include "SoundSystem.h"


SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
}

SoundSystem::SoundSystem(std::vector<std::shared_ptr<AudioComponent>> components)
{
	_components = components;
}

std::string SoundSystem::GetComponentPath(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->_path;
}

void SoundSystem::SetComponentPath(std::shared_ptr<AudioComponent> audioComponent, std::string path)
{
	audioComponent->_path = path;
}

bool SoundSystem::GetComponentMute(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->_mute;
}

void SoundSystem::SetComponentMute(std::shared_ptr<AudioComponent> audioComponent, bool muteState)
{
	audioComponent->_mute = muteState;
}

bool SoundSystem::GetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->_playOnAwake;
}

void SoundSystem::SetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent, bool playOnAwakeState)
{
	audioComponent->_playOnAwake = playOnAwakeState;
}

bool SoundSystem::GetComponentLoop(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->_loop;
}

void SoundSystem::SetComponentLoop(std::shared_ptr<AudioComponent> audioComponent, bool loopState)
{
	audioComponent->_loop = loopState;
}

float SoundSystem::GetComponentVolume(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->_volume;
}

void SoundSystem::SetComponentVolume(std::shared_ptr<AudioComponent> audioComponent, float volume)
{
	audioComponent->_volume = volume;
}

void SoundSystem::Iterate()
{
	for each (std::shared_ptr<AudioComponent> component in _components)
	{

	}
}
