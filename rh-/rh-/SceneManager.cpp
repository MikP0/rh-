#include "pch.h"
#include "SceneManager.h"


SceneManager::SceneManager()
{
}

std::shared_ptr<Scene> SceneManager::GetScene(std::string sceneName)
{
	return _scenes.at(sceneName);
}

