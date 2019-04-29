#pragma once

#include "Component.h"
#include "Audio.h"

using namespace std;

class AudioComponent : public Component
{
public:
	string Path;
	bool Mute, Loop;
	float Volume, Pitch, Pan, RemainingDelayTime;
	float DelayTimeLimit;
	unique_ptr<DirectX::SoundEffect> AudioFile;

	AudioComponent(string path, float delayTime);
	~AudioComponent();

	virtual ComponentType GetType(void) override;
};

