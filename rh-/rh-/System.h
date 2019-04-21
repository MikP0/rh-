//
// System.h
//

#pragma once
#include "pch.h"
#include "Component.h"

typedef std::shared_ptr<Component> ComponentPtr;

class System
{
public:
	System() {};
	virtual ~System() {};
	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) = 0;
	virtual void UpdateComponentsCollection() = 0;

protected:
	ComponentType _componentsType;
	virtual void Iterate() = 0;
	virtual void Initialize() = 0;
};
