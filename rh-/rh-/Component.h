#pragma once

#include "Entity.h"
#include <string>

struct ComponentType {
	ComponentType(std::string name) 
	{
		_name = name;
	}

	std::string _name;
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
	std::shared_ptr<Entity> _parentEntity;

private:
	bool _isEnabled;
};

