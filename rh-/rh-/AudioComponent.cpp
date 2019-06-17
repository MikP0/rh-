#include "pch.h"
#include "AudioComponent.h"

AudioComponent::AudioComponent(string path)
{
	SetIsEnabled(true);
	IsInstanced = true;
	Path = path;
	Mute = true;
	Loop = false;
	Volume = 1.0f;
	Pitch = 0.0f;
	Pan = 0.0f;
	Delay = 0.0f;
	AudioFile = nullptr;
	AudioLoopInstance = nullptr;
}

AudioComponent::AudioComponent(string path, bool isInstanced)
{
	SetIsEnabled(true);
	IsInstanced = isInstanced;
	Path = path;
	Mute = true;
	Loop = false;
	Volume = 1.0f;
	Pitch = 0.0f;
	Pan = 0.0f;
	Delay = 0.0f;
	AudioFile = nullptr;
	AudioLoopInstance = nullptr;
}

AudioComponent::AudioComponent(const std::shared_ptr<AudioComponent> audioComponent)
{
	SetIsEnabled(true);
	IsInstanced = true;
	Path = audioComponent->Path;
	Mute = audioComponent->Mute;
	Loop = audioComponent->Loop;
	Volume = audioComponent->Volume;
	Pitch = audioComponent->Pitch;
	Pan = audioComponent->Pan;
	Delay = audioComponent->Delay;
	AudioFile = nullptr;
	AudioLoopInstance = nullptr;
}

AudioComponent::AudioComponent(const std::shared_ptr<AudioComponent> audioComponent, bool isInstanced)
{
	SetIsEnabled(true);
	IsInstanced = isInstanced;
	Path = audioComponent->Path;
	Mute = audioComponent->Mute;
	Loop = audioComponent->Loop;
	Volume = audioComponent->Volume;
	Pitch = audioComponent->Pitch;
	Pan = audioComponent->Pan;
	Delay = audioComponent->Delay;
	AudioFile = nullptr;
	AudioLoopInstance = nullptr;
}

AudioComponent::~AudioComponent()
{
}