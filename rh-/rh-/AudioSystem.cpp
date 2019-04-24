#include "pch.h"
#include "AudioSystem.h"


AudioSystem::AudioSystem(std::shared_ptr<EntityManager> entityManager) : System(entityManager)
{
	_componentsType._name = "Audio";
}

AudioSystem::~AudioSystem()
{
}

std::string AudioSystem::GetComponentPath(AudioComponentPtr audioComponent)
{
	return audioComponent->Path;
}

void AudioSystem::SetComponentPath(AudioComponentPtr audioComponent, std::string path)
{
	audioComponent->Path = path;
}

bool AudioSystem::GetComponentMute(AudioComponentPtr audioComponent)
{
	return audioComponent->Mute;
}

void AudioSystem::SetComponentMute(AudioComponentPtr audioComponent, bool muteState)
{
	audioComponent->Mute = muteState;
}

bool AudioSystem::GetComponentPlayOnAwake(AudioComponentPtr audioComponent)
{
	return audioComponent->PlayOnAwake;
}

void AudioSystem::SetComponentPlayOnAwake(AudioComponentPtr audioComponent, bool playOnAwakeState)
{
	audioComponent->PlayOnAwake = playOnAwakeState;
}

bool AudioSystem::GetComponentLoop(AudioComponentPtr audioComponent)
{
	return audioComponent->Loop;
}

void AudioSystem::SetComponentLoop(AudioComponentPtr audioComponent, bool loopState)
{
	audioComponent->Loop = loopState;
}

float AudioSystem::GetComponentVolume(AudioComponentPtr audioComponent)
{
	return audioComponent->Volume;
}

void AudioSystem::SetComponentVolume(AudioComponentPtr audioComponent, float volume)
{
	audioComponent->Volume = volume;
}

std::vector<ComponentPtr> AudioSystem::GetComponents(ComponentType componentType)
{
	std::vector<ComponentPtr> allComponents;
	std::vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (std::dynamic_pointer_cast<AudioComponent>(component)->GetType()._name.compare(componentType._name) == 0)
		{
			selectedComponents.push_back(component);
		}
	}

	return selectedComponents;
}

void AudioSystem::UpdateComponentsCollection()
{
	_components.clear();
	std::vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		AudioComponentPtr audioComponent = std::dynamic_pointer_cast<AudioComponent>(component);
		_components.push_back(audioComponent);
	}
}

void AudioSystem::Initialize()
{
}

void AudioSystem::Iterate()
{
	for each (AudioComponentPtr component in _components)
	{
		
	}
}
