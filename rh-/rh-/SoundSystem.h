#pragma once

#include <vector>
#include <string>
#include "System.h"
#include "AudioComponent.h"

typedef std::shared_ptr<AudioComponent> AudioComponentPtr;

class SoundSystem : public System
{
public:
	SoundSystem();
	SoundSystem(std::vector<std::shared_ptr<AudioComponent>> components);
	~SoundSystem();

	std::string GetComponentPath(std::shared_ptr<AudioComponent> audioComponent);
	void SetComponentPath(std::shared_ptr<AudioComponent> audioComponent, std::string path);
	bool GetComponentMute(std::shared_ptr<AudioComponent> audioComponent);
	void SetComponentMute(std::shared_ptr<AudioComponent> audioComponent, bool muteState);
	bool GetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent);
	void SetComponentPlayOnAwake(std::shared_ptr<AudioComponent> audioComponent, bool playOnAwakeState);
	void SetComponentLoop(std::shared_ptr<AudioComponent> audioComponent, bool loopState);
	bool GetComponentLoop(std::shared_ptr<AudioComponent> audioComponent);
	float GetComponentVolume(std::shared_ptr<AudioComponent> audioComponent);
	void SetComponentVolume(std::shared_ptr<AudioComponent> audioComponent, float volume);

	virtual void Iterate() override;

private:
	std::vector<std::shared_ptr<AudioComponent>> _components;

};

