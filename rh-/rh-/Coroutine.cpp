#include "Coroutine.h"

float Coroutine::elapsedTime = 0;

Coroutine::~Coroutine()
{
}

Coroutine::Coroutine()
{
	this->waitTime = 0;
	this->active = false;
}

Coroutine::Coroutine(float time)
{
	this->waitTime = time;
	this->active = true;
}

bool Coroutine::Update()
{
	if (waitTime > 0)
	{
		this->waitTime -= Coroutine::GetElapsedTime();
		return true;
	}
	else
	{
		this->active = false;
		return false;
	}
}

float Coroutine::GetTime()
{
	return this->waitTime;
}

void Coroutine::Restart(float time)
{
	this->waitTime = time;
	this->active = true;
}

void Coroutine::UpdateElapsedTime(float Time)
{
	Coroutine::elapsedTime = Time;
}

float Coroutine::GetElapsedTime()
{
	return Coroutine::elapsedTime;
}
