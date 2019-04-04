#include "pch.h"
#include "Component.h"


Component::Component()
{
}


Component::~Component()
{
}

std::shared_ptr<Entity> Component::GetParent() const
{
	return _parentEntity;
}

void Component::SetParent(std::shared_ptr<Entity> parent)
{
	_parentEntity = parent;
}

bool Component::CheckIfEnabled()
{
	return _isEnabled;
}
