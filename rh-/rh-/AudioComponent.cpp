#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent()
{
	SetIsEnabled(true);
	Mute = false;
	PlayOnAwake = true;
	Loop = true;
}

AudioComponent::AudioComponent(std::string path)
{
	SetIsEnabled(true);
	Path = path;
	Mute = false;
	PlayOnAwake = true;
	Loop = true;
}

AudioComponent::~AudioComponent()
{
}

ComponentType AudioComponent::GetType(void)
{
	return ComponentType("Audio");
}
