#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent(string path, float delayTime)
{
	SetIsEnabled(true);
	Path = path;
	Mute = true;
	Loop = false;
	Volume = 1.0f;
	Pitch = 0.0f;
	Pan = 0.0f;
	AudioFile = nullptr;
	AudioLoopInstance = nullptr;
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
