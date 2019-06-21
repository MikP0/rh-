#pragma once

#include <vector>
#include <string>
#include "StepTimer.h"
#include "Audio.h"
#include "System.h"
#include "AudioComponent.h"

using namespace std;
using namespace DirectX;

typedef shared_ptr<AudioComponent> AudioComponentPtr;

class AudioSystem : public System
{
public:

	AudioSystem();
	~AudioSystem();

	string GetComponentPath(AudioComponentPtr audioComponent);
	void SetComponentPath(AudioComponentPtr audioComponent, string path);
	bool GetComponentMute(AudioComponentPtr audioComponent);
	void SetComponentMute(AudioComponentPtr audioComponent, bool muteState);
	void SetComponentLoop(AudioComponentPtr audioComponent, bool loopState);
	bool GetComponentLoop(AudioComponentPtr audioComponent);
	float GetComponentVolume(AudioComponentPtr audioComponent);
	void SetComponentVolume(AudioComponentPtr audioComponent, float volume);
	void SetComponentAudioFile(AudioComponentPtr audioComponent);
	void Suspend();
	void Resume();
	void RetryAudio();
	void PlayAudio(AudioComponentPtr audioComponent);
	void StopAudio(AudioComponentPtr audioComponent);
	void StopAudioImmediately(AudioComponentPtr audioComponent);
	void PauseAudio(AudioComponentPtr audioComponent);
	void ResumeAudio(AudioComponentPtr audioComponent);

	virtual void Initialize() override;
	virtual void Iterate() override;

	static float VOLUME;

private:
	unique_ptr<DirectX::AudioEngine> _audioEngine;
	bool _retryAudio;
	const float LOOP_DELAY_TIME = 0.2;
	const float LOOP_DELAY_STEP = 0.02;	
};

