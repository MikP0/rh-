#pragma once

#include <vector>
#include <string>
#include "System.h"
#include "AudioComponent.h"

typedef std::shared_ptr<AudioComponent> AudioComponentPtr;

class AudioSystem : public System
{
public:
	AudioSystem();
	~AudioSystem();

	std::string GetComponentPath(AudioComponentPtr audioComponent);
	void SetComponentPath(AudioComponentPtr audioComponent, std::string path);
	bool GetComponentMute(AudioComponentPtr audioComponent);
	void SetComponentMute(AudioComponentPtr audioComponent, bool muteState);
	bool GetComponentPlayOnAwake(AudioComponentPtr audioComponent);
	void SetComponentPlayOnAwake(AudioComponentPtr audioComponent, bool playOnAwakeState);
	void SetComponentLoop(AudioComponentPtr audioComponent, bool loopState);
	bool GetComponentLoop(AudioComponentPtr audioComponent);
	float GetComponentVolume(AudioComponentPtr audioComponent);
	void SetComponentVolume(AudioComponentPtr audioComponent, float volume);

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

protected:
	virtual void Iterate() override;

private:
	ComponentType _componentsType;
	std::vector<AudioComponentPtr> _components;
};

