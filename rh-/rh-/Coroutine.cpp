#include "Coroutine.h"

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

bool Coroutine::Update(float elapsedTime)
{
	if (waitTime > 0)
	{
		this->waitTime -= elapsedTime;
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