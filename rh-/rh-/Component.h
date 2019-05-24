#pragma once

#include "Entity.h"
#include <string>


class Component
{
public:
	Component();
	virtual ~Component() = default;

	std::shared_ptr<Entity> GetParent() const;
	bool CheckIfEnabled();

	void SetParent(std::shared_ptr<Entity> parent);
	void SwitchEnabled(void);
	void SetIsEnabled(bool flag);

protected:
	std::shared_ptr<Entity> _parentEntity;

private:
	bool _isEnabled;
};

