#pragma once

#include <vector>
#include <map>
#include "pch.h"

using namespace std;
using namespace DirectX;

enum actionList
{
	none,
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
	anchorRotation,

	moveFor,
	moveBac,
	moveLeft,
	moveRight,
	special1,
	special2,
	special3,
	special4,

	playBackground,
	playSound1,

	freeCamera,

	debugDrawAll,
	debugDrawWithoutRegions
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
	z,
	x,
	c,
	v,
	b,
	upperArrow,
	leftArrow,
	lowerArrow,
	rightArrow,
	space,
	leftControl,
	lpm,
	rpm,
	scrollPush,
};

class Input
{
public:
	static map<availableKeys, actionList> AvailableKeysActionsBinding;

	static Keyboard::State GetKeyboardState();
	static Mouse::State GetMouseState();

	static map<availableKeys, actionList> GetActionKeysBindings();
	static vector<actionList> GetActions();
	static map<availableKeys, bool> GetPushedKeys();
	static SimpleMath::Vector2 GetMousePosition();
	static void SetWindowForMouse(HWND window);
	static void SetMouseMode(DirectX::Mouse::Mode mode);
	static void ResetWheel();

private:
	static unique_ptr<Keyboard> _keyboard;
	static unique_ptr<Mouse> _mouse;

	Input() {};
	~Input() {};
};

