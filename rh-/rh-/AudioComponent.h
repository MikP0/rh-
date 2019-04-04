#pragma once

#include "Component.h"

class AudioComponent : public Component
{
public:
	AudioComponent();
	AudioComponent(std::string path);
	~AudioComponent();

	std::string GetPath();
	void SetPath(std::string path);
	bool GetMute();
	void SetMute(bool muteState);
	bool GetPlayOnAwake();
	void SetPlayOnAwake(bool playOnAwakeState);
	void SetLoop(bool loopState);
	bool GetLoop();
	float GetVolume();
	void SetVolume(float volume);

	virtual ComponentType GetType(void) override;

private:
	std::string _path;
	bool _mute, _playOnAwake, _loop;
	float _volume;
};

