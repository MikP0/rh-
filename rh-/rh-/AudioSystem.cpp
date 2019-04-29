#include "pch.h"
#include "AudioSystem.h"


AudioSystem::AudioSystem()
{
	_componentsType.name = "Audio";
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
	#endif
	_audioEngine = std::make_unique<AudioEngine>(eflags);
	_retryAudio = false;
}

AudioSystem::~AudioSystem()
{
	if (_audioEngine)
	{
		_audioEngine->Suspend();
	}
}

std::string AudioSystem::GetComponentPath(AudioComponentPtr audioComponent)
{
	return audioComponent->Path;
}

void AudioSystem::SetComponentPath(AudioComponentPtr audioComponent, string path)
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

void AudioSystem::Update()
{
	if (_retryAudio)
	{
		_retryAudio = false;
		if (_audioEngine->Reset())
		{
			// TODO: restart any looped sounds here
		}
	}
	else if (!_audioEngine->Update())
	{
		if (_audioEngine->IsCriticalError())
		{
			_retryAudio = true;
		}
	}
}

void AudioSystem::Suspend()
{
	_audioEngine->Suspend();
}

void AudioSystem::Resume()
{
	_audioEngine->Resume();
}

void AudioSystem::RetryAudio()
{
	_retryAudio = true;
}

std::vector<ComponentPtr> AudioSystem::GetComponents(ComponentType componentType)
{
	vector<ComponentPtr> allComponents;
	vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (dynamic_pointer_cast<AudioComponent>(component)->GetType().name.compare(componentType.name) == 0)
		{
			selectedComponents.push_back(component);
		}
	}

	return selectedComponents;
}

void AudioSystem::UpdateComponentsCollection()
{
	_components.clear();
	vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		AudioComponentPtr audioComponent = dynamic_pointer_cast<AudioComponent>(component);
		_components.push_back(audioComponent);
	}
}

void AudioSystem::Iterate()
{
	for each (AudioComponentPtr component in _components)
	{
		
	}
}
