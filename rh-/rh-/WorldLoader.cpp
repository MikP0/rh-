#include "pch.h"
#include "WorldLoader.h"


WorldLoader::WorldLoader(std::shared_ptr<World> world)
{
	_world = world;
}

WorldLoader::~WorldLoader()
{
}

void WorldLoader::LoadWorldFromXML(std::string filename)
{

}
