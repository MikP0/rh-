#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent()
{
	SetIsEnabled(true);
	Mute = false;
	PlayOnAwake = true;
	Loop = true;
}

AudioComponent::AudioComponent(string path)
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
	ComponentType componentType;
	componentType.name = "Audio";
	return componentType;
}
