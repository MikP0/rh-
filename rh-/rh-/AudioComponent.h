#pragma once

#include "Component.h"

using namespace std;

class AudioComponent : public Component
{
public:
	string Path;
	bool Mute, PlayOnAwake, Loop;
	float Volume;

	AudioComponent();
	AudioComponent(string path);
	~AudioComponent();

	virtual ComponentType GetType(void) override;
	
};

