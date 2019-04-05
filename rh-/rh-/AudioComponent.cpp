#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent()
{
	Mute = false;
	PlayOnAwake = true;
	Loop = true;
}

AudioComponent::AudioComponent(std::string path)
{
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
	ComponentType componentType;
	componentType.name = "Audio";
	return componentType;
}
