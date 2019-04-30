#pragma once

#include "Component.h"
#include "Audio.h"

using namespace std;

class AudioComponent : public Component
{
public:
	string Path;
	bool Mute, Loop;
	float Volume, Pitch, Pan;
	unique_ptr<DirectX::SoundEffect> AudioFile;
	unique_ptr<DirectX::SoundEffectInstance> AudioLoopInstance;

	AudioComponent(string path, float delayTime);
	~AudioComponent();

	virtual ComponentType GetType(void) override;
};

