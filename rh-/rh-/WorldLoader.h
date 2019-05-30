#pragma once
#include "World.h"

class WorldLoader
{
public:
	WorldLoader(std::shared_ptr<World> world);
	virtual ~WorldLoader();
	void LoadWorldFromXML(std::string filename);

private:
	std::shared_ptr<World> _world;
};

