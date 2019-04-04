#pragma once

#include "Component.h"

class AudioComponent : public Component
{
public:
	std::string _path;
	bool _mute, _playOnAwake, _loop;
	float _volume;

	AudioComponent();
	AudioComponent(std::string path);
	~AudioComponent();

	virtual ComponentType GetType(void) override;
	
};

