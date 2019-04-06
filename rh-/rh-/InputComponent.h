#pragma once

#include <map>
#include "Component.h"

enum actionList
{
	dodge,
	normal,
	strong,
	spin,
	bite,
	dash,
	cleave,
	swap,
	fear,

	closeWindow,
	up,
	down,
	left,
	right,
	forward,
	backward,
	anchorRotation
};

enum availableKeys
{
	one,
	two,
	three,
	four,
	shift,
	q,
	e,
	esc,
	w,
	a,
	s,
	d,
	upperArrow,
	leftArrow,
	lowerArrow,
	rightArrow,
	space,
	leftControl,
	lpm,
	rpm,
	scrollPush
};

class InputComponent : public Component
{
public:
	std::map<availableKeys, actionList> SkillsKeys;

	InputComponent();
	InputComponent(std::map<availableKeys, actionList> actionSet);
	~InputComponent();

	virtual ComponentType GetType(void) override;
};

