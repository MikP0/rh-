#include "pch.h"
#include "AudioSystem.h"


AudioSystem::AudioSystem(std::shared_ptr<EntityManager> entityManager) : System(entityManager)
{
	_componentsType.name = "Audio";
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

void AudioSystem::UpdateTime(float time)
{
	_elapsedTime = time;
	_delayTime -= _elapsedTime;
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
	audioComponent->Mute = true;
	audioComponent->AudioLoopInstance->Stop(false);
}

void AudioSystem::StopAudioImmediately(AudioComponentPtr audioComponent)
{
	audioComponent->Mute = true;
	audioComponent->AudioLoopInstance->Stop(true);
}

void AudioSystem::PauseAudio(AudioComponentPtr audioComponent)
{
	audioComponent->Mute = true;
	audioComponent->AudioLoopInstance->Pause();
}

void AudioSystem::ResumeAudio(AudioComponentPtr audioComponent)
{
	audioComponent->Mute = false;
	audioComponent->AudioLoopInstance->Resume();
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

void AudioSystem::InsertComponent(AudioComponentPtr component)
{
	_components.push_back(component);
	SetComponentAudioFile(component);
}

void AudioSystem::Initialize()
{
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
		
		/*if (shouldResetAudio)
		{
			PlayAudio(component);
			continue;
		}*/

		if (component->Loop)
		{
			if (soundState == STOPPED)
			{
				if (!component->Mute)
				{
					PlayAudio(component);
					component->Mute = true;
					continue;
				}

				component->Mute = true;
				continue;
			}
			
			/*if (soundState == PLAYING)
			{
				if (!component->Mute)
				{
					PauseAudio(component);
					continue;
				}
				
				component->Mute = true;
				continue;
			}*/

			/*if (soundState == PAUSED)
			{
				if (!component->Mute)
				{
					ResumeAudio(component);
					continue;
				}
				
				component->Mute = true;
				continue;
			}*/
		}
		else
		{
			if (soundState == STOPPED)
			{
				if (!component->Mute)
				{
					PlayAudio(component);
					continue;
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
