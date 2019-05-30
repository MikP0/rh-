#include "pch.h"
#include "WorldLoader.h"

#include "tinyxml2.h"


WorldLoader::WorldLoader(std::shared_ptr<World> world)
{
	_world = world;
}

WorldLoader::~WorldLoader()
{
}

void WorldLoader::LoadWorldFromXML(std::string filename)
{
	tinyxml2::XMLDocument xmlLevel;
}
