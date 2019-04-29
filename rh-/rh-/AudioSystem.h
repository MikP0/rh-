#pragma once

#include <vector>
#include <string>
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
	bool GetComponentPlayOnAwake(AudioComponentPtr audioComponent);
	void SetComponentPlayOnAwake(AudioComponentPtr audioComponent, bool playOnAwakeState);
	void SetComponentLoop(AudioComponentPtr audioComponent, bool loopState);
	bool GetComponentLoop(AudioComponentPtr audioComponent);
	float GetComponentVolume(AudioComponentPtr audioComponent);
	void SetComponentVolume(AudioComponentPtr audioComponent, float volume);
	void Update();
	void Suspend();
	void Resume();
	void RetryAudio();

	virtual vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

protected:
	virtual void Iterate() override;

private:
	vector<AudioComponentPtr> _components;
	unique_ptr<DirectX::AudioEngine> _audioEngine;
	bool _retryAudio;
};

