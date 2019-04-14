#pragma once
#include "Component.h"

class ComponentFactory
{
public:
	ComponentFactory();
	~ComponentFactory();

	Component CreateComponent(int entityId)
};

