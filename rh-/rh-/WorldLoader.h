#pragma once

#include "../tinyxml2-master/tinyxml2.h"

#include "World.h"
#include "Camera.h"

#include "RenderableComponent.h"
#include "LightComponent.h"
#include "AudioComponent.h"
#include "PhysicsComponent.h"

class WorldLoader
{
public:
	WorldLoader(std::shared_ptr<World> world, Camera* camera);
	virtual ~WorldLoader();
	void LoadWorldFromXML(std::string filename);

private:
	std::shared_ptr<World> _world;
	Camera* _camera;
};

