#include "pch.h"
#include "Scene.h"


Scene::Scene()
{
	_sceneEntity = std::make_shared<Entity>();
}


Scene::~Scene()
{
}

void Scene::UpdateScene(void)
{
	_sceneEntity->Update();
}

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	_sceneEntity->AddChild(entity);
}
