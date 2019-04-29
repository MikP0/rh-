#include "pch.h"
#include "AudioComponent.h"


AudioComponent::AudioComponent(string path, float delayTime) : DelayTimeLimit(delayTime)
{
	SetIsEnabled(true);
	Path = path;
	Mute = true;
	Loop = true;
	Volume = 1.0f;
	Pitch = 0.0f;
	Pan = 0.0f;
	RemainingDelayTime = 0.0f;
	AudioFile = nullptr;
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
