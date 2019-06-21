#include "pch.h"
#include "Coroutine.h"

float Coroutine::elapsedTime = 0;

Coroutine::~Coroutine()
{
}

Coroutine::Coroutine()
{
	this->waitTime = 0;
	this->eventTime = 0;
	this->active = false;
	this->corutineEvent = false;
	this->eventDone = true;
}

Coroutine::Coroutine(float time)
{
	this->waitTime = time;
	this->eventTime = 0;
	this->active = true;
	this->corutineEvent = false;
	this->eventDone = true;
}

Coroutine::Coroutine(float time, float eventTime)
{
	this->waitTime = time;
	this->eventTime = eventTime;
	this->active = true;
	this->corutineEvent = false;
	this->eventDone = true;
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

bool Coroutine::UpdateEvent()
{
	if (active)
	{
		if (!eventDone)
		{
			if (eventTime > 0)
			{
				this->eventTime -= Coroutine::GetElapsedTime();
				return true;
			}
			else
			{
				this->eventDone = true;
				return false;
			}
		}
	}
	return true;
}

float Coroutine::GetTime()
{
	return this->waitTime;
}

void Coroutine::Restart(float time)
{
	this->waitTime = time;
	this->active = true;
	this->eventDone = true;
}

void Coroutine::RestartWithEvent(float time, float eventTime)
{
	this->waitTime = time;
	this->eventTime = eventTime;
	this->eventDone = false;
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
