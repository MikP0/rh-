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
	return audioComponent->Path;
}

void SoundSystem::SetComponentPath(std::shared_ptr<AudioComponent> audioComponent, std::string path)
{
	audioComponent->Path = path;
}

bool SoundSystem::GetComponentMute(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->Mute;
}

void SoundSystem::SetComponentMute(std::shared_ptr<AudioComponent> audioComponent, bool muteState)
{
	audioComponent->Mute = muteState;
}

bool SoundSystem::GetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->PlayOnAwake;
}

void SoundSystem::SetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent, bool playOnAwakeState)
{
	audioComponent->PlayOnAwake = playOnAwakeState;
}

bool SoundSystem::GetComponentLoop(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->Loop;
}

void SoundSystem::SetComponentLoop(std::shared_ptr<AudioComponent> audioComponent, bool loopState)
{
	audioComponent->Loop = loopState;
}

float SoundSystem::GetComponentVolume(std::shared_ptr<AudioComponent> audioComponent)
{
	return audioComponent->Volume;
}

void SoundSystem::SetComponentVolume(std::shared_ptr<AudioComponent> audioComponent, float volume)
{
	audioComponent->Volume = volume;
}

void SoundSystem::Iterate()
{
	for each (std::shared_ptr<AudioComponent> component in _components)
	{

	}
}
