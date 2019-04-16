#pragma once
#include "Entity.h"

class Scene
{
public:
	Scene();
	~Scene();
	
	void UpdateScene(void);
	void AddEntity(std::shared_ptr<Entity> entity);

private:
	std::shared_ptr<Entity> _sceneEntity;
};

