#include "pch.h"
#include "AudioSystem.h"


AudioSystem::AudioSystem(shared_ptr<EntityManager> entityManager) : System(entityManager)
{
	_componentsType._name = "Audio";
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
	#endif
	_audioEngine = make_unique<AudioEngine>(eflags);
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

void AudioSystem::SetComponentAudioFile(AudioComponentPtr audioComponent)
{
	wstring wide_string = wstring(audioComponent->Path.begin(), audioComponent->Path.end());
	const wchar_t* result = wide_string.c_str();
	audioComponent->AudioFile = make_unique<SoundEffect>(_audioEngine.get(), result);
	audioComponent->AudioLoopInstance = audioComponent->AudioFile->CreateInstance();
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

void AudioSystem::PlayAudio(AudioComponentPtr audioComponent)
{
	audioComponent->AudioLoopInstance->SetVolume(audioComponent->Volume);
	audioComponent->AudioLoopInstance->SetPitch(audioComponent->Pitch);
	audioComponent->AudioLoopInstance->SetPan(audioComponent->Pan);
	audioComponent->AudioLoopInstance->Play(audioComponent->Loop);
}

void AudioSystem::StopAudio(AudioComponentPtr audioComponent)
{
	audioComponent->AudioLoopInstance->Stop(false);
}

void AudioSystem::StopAudioImmediately(AudioComponentPtr audioComponent)
{
	audioComponent->AudioLoopInstance->Stop(true);
}

void AudioSystem::PauseAudio(AudioComponentPtr audioComponent)
{
	audioComponent->AudioLoopInstance->Pause();
}

void AudioSystem::ResumeAudio(AudioComponentPtr audioComponent)
{
	audioComponent->AudioLoopInstance->Resume();
}

// zastanowic sie czy ta funkcja nie jest do usuniecia z System
std::vector<ComponentPtr> AudioSystem::GetComponents(ComponentType componentType)
{
	//vector<ComponentPtr> allComponents;
	//vector<ComponentPtr> selectedComponents;
	////allComponents = componentManager.GetAllComponents();

	//for each (ComponentPtr component in allComponents)
	//{
	//	if (dynamic_pointer_cast<AudioComponent>(component)->GetType()._name.compare(componentType._name) == 0)
	//	{
	//		selectedComponents.push_back(component);
	//	}
	//}

	//return selectedComponents;

	vector<ComponentPtr> result = _entityManager->GetComponents(_componentsType);

	return result;
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

void AudioSystem::InsertComponent(AudioComponentPtr component)
{
	_components.push_back(component);
	SetComponentAudioFile(component);
}

void AudioSystem::Initialize()
{
	int backgroundAudioEntityID = _entityManager->CreateEntity("BackgroundAudioEntity");
	int sound1AudioEntityID = _entityManager->CreateEntity("Sound1AudioEntity");

	shared_ptr<AudioComponent> backgroundAudioComponent = std::make_shared<AudioComponent>("Resources\\Audio\\In The End.wav");
	backgroundAudioComponent->Loop = true;
	SetComponentAudioFile(backgroundAudioComponent);
	shared_ptr<AudioComponent> sound1AudioComponent = make_shared<AudioComponent>("Resources\\Audio\\KnifeSlice.wav");
	SetComponentAudioFile(sound1AudioComponent);

	_entityManager->AddComponent(backgroundAudioEntityID, backgroundAudioComponent);
	_entityManager->AddComponent(sound1AudioEntityID, sound1AudioComponent);

	vector<shared_ptr<Component>> components = _entityManager->GetComponents(ComponentType("Audio"));

	for each (shared_ptr<Component> component in components)
	{
		_components.push_back(dynamic_pointer_cast<AudioComponent>(component));
	}
	auto test = _components;
}

void AudioSystem::Iterate()
{
	bool shouldResetAudio = false;

	if (_retryAudio)
	{
		_retryAudio = false;

		if (_audioEngine->Reset())
		{
			// TODO: restart any looped sounds here
			shouldResetAudio = true;
		}
	}
	else if (!_audioEngine->Update())
	{
		if (_audioEngine->IsCriticalError())
		{
			_retryAudio = true;
		}
	}

	for each (AudioComponentPtr component in _components)
	{
		SoundState soundState = component->AudioLoopInstance->GetState();
		
		if (shouldResetAudio)
		{
			PlayAudio(component);
			continue;
		}

		if (component->Loop)
		{
			if (soundState == STOPPED)
			{
				if (!component->Mute)
				{
					PlayAudio(component);
					component->Delay = LOOP_DELAY_TIME;
				}

				component->Mute = true;
				continue;
			}
			
			if (soundState == PLAYING)
			{
				if (component->Delay > 0)
				{
					component->Delay -= LOOP_DELAY_STEP;
				}

				if (!component->Mute && component->Delay <= 0)
				{
					PauseAudio(component);
					component->Delay = LOOP_DELAY_TIME;
				}
				
				component->Mute = true;
				continue;
			}

			if (soundState == PAUSED)
			{
				if (component->Delay > 0)
				{
					component->Delay -= LOOP_DELAY_STEP;
				}

				if (!component->Mute && component->Delay <= 0)
				{
					ResumeAudio(component);
					component->Delay = LOOP_DELAY_TIME;
					continue;
				}
				
				component->Mute = true;
				continue;
			}
		}
		else
		{
			if (soundState == STOPPED)
			{
				if (!component->Mute)
				{
					PlayAudio(component);
				}

				component->Mute = true;
				continue;
			}
			
			if (soundState == PLAYING)
			{
				component->Mute = true;
				continue;
			}
		}
	}

	
}
