//
// System.h
//

#pragma once
#include "pch.h"

class World;

class System
{
public:
	System() {};
	virtual ~System() {};
	virtual void Iterate() = 0;
	virtual void Initialize() = 0;
	void SetWorld(std::shared_ptr<World> world) { _world = world; }

protected:
	std::shared_ptr<World> _world;
};
