#pragma once

#include "pch.h"

class Coroutine
{
public:

	~Coroutine();
	Coroutine();
	Coroutine(float time);
	// return TRUE if still active and waitTime > 0
	bool Update();
	float GetTime();
	void Restart(float time);

	float waitTime;
	bool active;

	static float elapsedTime;
	static void UpdateElapsedTime(float Time);
	static float GetElapsedTime();
};

