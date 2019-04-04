//
// System.h
//

#pragma once
#include "pch.h"
#include "Component.h"

class System
{
public:
	System();
	virtual ~System();
	virtual std::vector<std::shared_ptr<Component>> GetComponents() = 0;

protected:
	virtual void Iterate() = 0;
	std::vector<std::shared_ptr<Component>> _components;
};
