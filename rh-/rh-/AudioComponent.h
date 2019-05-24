#pragma once

#include "Component.h"
#include "Audio.h"

using namespace std;

class AudioComponent : public Component
{
public:
	string Path;
	bool Mute, Loop;
	float Volume, Pitch, Pan, Delay;
	unique_ptr<DirectX::SoundEffect> AudioFile;
	unique_ptr<DirectX::SoundEffectInstance> AudioLoopInstance;

	AudioComponent(string path);
	AudioComponent(const std::shared_ptr<AudioComponent> audioComponent);
	~AudioComponent();
};

