#include "pch.h"
#include "AudioSystem.h"

float AudioSystem::VOLUME = 1.0f;

AudioSystem::AudioSystem()
{
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
	
	if (audioComponent->IsInstanced)
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
	if (audioComponent->IsInstanced)
	{
		audioComponent->AudioLoopInstance->SetVolume(audioComponent->Volume);
		audioComponent->AudioLoopInstance->SetPitch(audioComponent->Pitch);
		audioComponent->AudioLoopInstance->SetPan(audioComponent->Pan);
		audioComponent->AudioLoopInstance->Play(audioComponent->Loop);
	}
	else
	{
		audioComponent->AudioFile->Play(
			audioComponent->Volume, audioComponent->Pitch, audioComponent->Pan);
	}
}

void AudioSystem::StopAudio(AudioComponentPtr audioComponent)
{
	if (audioComponent->IsInstanced)
		audioComponent->AudioLoopInstance->Stop(false);
}

void AudioSystem::StopAudioImmediately(AudioComponentPtr audioComponent)
{
	if (audioComponent->IsInstanced)
		audioComponent->AudioLoopInstance->Stop(true);
}

void AudioSystem::PauseAudio(AudioComponentPtr audioComponent)
{
	if (audioComponent->IsInstanced)
		audioComponent->AudioLoopInstance->Pause();
}

void AudioSystem::ResumeAudio(AudioComponentPtr audioComponent)
{
	if (audioComponent->IsInstanced)
		audioComponent->AudioLoopInstance->Resume();
}

void AudioSystem::Initialize()
{
	for (auto component : _world->GetComponents<AudioComponent>())
	{
		SetComponentAudioFile(component);
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

	for each (AudioComponentPtr component in _world->GetComponents<AudioComponent>())
	{
		if (shouldResetAudio)
		{
			PlayAudio(component);
			continue;
		}

		if (component->IsInstanced)
		{
			SoundState soundState = component->AudioLoopInstance->GetState();

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
		else
		{
			if (!component->Mute)
			{
				PlayAudio(component);
				component->Mute = true;
				continue;
			}
		}
		
	}

	
}
