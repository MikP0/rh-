#pragma once

#include "Component.h"

class AudioComponent : public Component
{
public:
	std::string Path;
	bool Mute, PlayOnAwake, Loop;
	float Volume;

	AudioComponent();
	AudioComponent(std::string path);
	~AudioComponent();

	virtual ComponentType GetType(void) override;
	
};

