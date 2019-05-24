#include "pch.h"
#include "Component.h"


Component::Component()
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

void Component::SwitchEnabled(void)
{
	_isEnabled = !_isEnabled;
}

void Component::SetIsEnabled(bool flag)
{
	_isEnabled = flag;
}

bool Component::CheckIfEnabled()
{
	return _isEnabled;
}
