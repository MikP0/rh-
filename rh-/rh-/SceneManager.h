#pragma once
#include <map>

#include "Scene.h"

class SceneManager
{
public:

	//SceneManager();
	virtual ~SceneManager() {};

	static SceneManager& GetInstance()
	{
		static SceneManager instance;

		return instance;
	}

	std::shared_ptr<Scene> GetScene(std::string sceneName);

private:
	SceneManager() {}
	SceneManager(SceneManager const&);  // Don't Implement
	void operator=(SceneManager const&); // Don't implement

	std::map<std::string, std::shared_ptr<Scene>> _scenes;
};

