#pragma once

#include "pch.h"

class Coroutine
{
public:

	~Coroutine();
	Coroutine();
	Coroutine(float time);
	Coroutine(float time, float eventTime);
	// return TRUE if still active and waitTime > 0
	bool Update();
	bool UpdateEvent();
	float GetTime();
	void Restart(float time);
	void RestartWithEvent(float time, float eventTime);

	float waitTime;
	bool active;

	float eventTime;
	bool corutineEvent;
	bool eventDone;

	static float elapsedTime;
	static void UpdateElapsedTime(float Time);
	static float GetElapsedTime();
};

