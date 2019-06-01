#pragma once

#include "pch.h"

class Coroutine
{
public:

	~Coroutine();
	Coroutine();
	Coroutine(float time);
	// return TRUE if still active and waitTime > 0
	bool Update(float elapsedTime);
	float GetTime();
	void Restart(float time);

	float waitTime;
	bool active;
};

