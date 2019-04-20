#pragma once

#include <map>

#include "Scene.h"

class SceneManager
{
public:

	SceneManager();
	virtual ~SceneManager() {};

	std::shared_ptr<Scene> GetScene(std::string sceneName);

private:

	std::map<std::string, std::shared_ptr<Scene>> _scenes;
};

