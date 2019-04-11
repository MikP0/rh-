#pragma once

#include "Entity.h"
#include <string>

struct ComponentType {
	std::string name;
};

class Component
{
public:
	Component();
	virtual ~Component();

	virtual ComponentType GetType(void) = 0;
	std::shared_ptr<Entity> GetParent() const;
	bool CheckIfEnabled();

	void SetParent(std::shared_ptr<Entity> parent);
	void SwitchEnabled(void);
	void SetIsEnabled(bool flag);

protected:
	bool _isEnabled;
	std::shared_ptr<Entity> _parentEntity;

};

