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
	void UpdateTime(float time);
	void Suspend();
	void Resume();
	void RetryAudio();
	void PlayAudio(AudioComponentPtr audioComponent);

	virtual vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

	void InsertComponent(AudioComponentPtr component); // temporary function for tests
	
	virtual void Iterate() override;

private:
	vector<AudioComponentPtr> _components;
	float _elapsedTime;
	unique_ptr<DirectX::AudioEngine> _audioEngine;
	bool _retryAudio;

};

