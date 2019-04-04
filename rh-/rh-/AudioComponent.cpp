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

ComponentType AudioComponent::GetType(void)
{
	ComponentType componentType;
	componentType.name = "Audio";
	return componentType;
}
