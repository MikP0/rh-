#include "pch.h"
#include "AudioSystem.h"


AudioSystem::AudioSystem()
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
std::vector<AudioComponentPtr> AudioSystem::GetComponents()
{
	vector<AudioComponentPtr> result = _world->GetComponents<AudioComponent>();

	return result;
}

void AudioSystem::UpdateComponentsCollection()
{
	_components.clear();
	_components = GetComponents();
}

void AudioSystem::InsertComponent(AudioComponentPtr component)
{
	_components.push_back(component);
	SetComponentAudioFile(component);
}

void AudioSystem::Initialize()
{
	Entity backgroundAudioEntityID = _world->CreateEntity("BackgroundAudioEntity");
	Entity sound1AudioEntityID = _world->CreateEntity("Sound1AudioEntity");

	shared_ptr<AudioComponent> backgroundAudioComponent = std::make_shared<AudioComponent>("Resources\\Audio\\In The End.wav");
	backgroundAudioComponent->Loop = true;
	SetComponentAudioFile(backgroundAudioComponent);
	shared_ptr<AudioComponent> sound1AudioComponent = make_shared<AudioComponent>("Resources\\Audio\\KnifeSlice.wav");
	SetComponentAudioFile(sound1AudioComponent);

	backgroundAudioEntityID.AddComponent<AudioComponent>(backgroundAudioComponent);
	sound1AudioEntityID.AddComponent<AudioComponent>(sound1AudioComponent);

	vector<shared_ptr<AudioComponent>> components = GetComponents();

	for each (shared_ptr<AudioComponent> component in components)
	{
		_components.push_back(component);
	}
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
