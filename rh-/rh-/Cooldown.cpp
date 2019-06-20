#include "pch.h"
#include "Cooldown.h"


Cooldown::Cooldown()
{
}

Cooldown::Cooldown(vector<string> skillsNames, vector<float> timeLimits)
{
	if (skillsNames.size() == timeLimits.size())
	{
		for (int i = 0; i < skillsNames.size(); i++)
		{
			CooldownUnit unit;
			unit.timeLimit = timeLimits[i];
			units[skillsNames[i]] = unit;
		}
	}
}

Cooldown::~Cooldown()
{
}

void Cooldown::StartSkillCounter(string skillName)
{
	units[skillName].canUse = false;
	units[skillName].remainigTime = units[skillName].timeLimit;
}

bool Cooldown::CanUseSkill(string skillName)
{
	return units[skillName].canUse;
}

float Cooldown::RemainingCooldownTime(string skillName)
{
	return units[skillName].remainigTime;
}

void Cooldown::Update()
{
	for (map<string, CooldownUnit>::iterator it = units.begin(); it != units.end(); it++)
	{
		if (!it->second.canUse)
		{
			if (it->second.remainigTime <= 0.0f)
			{
				it->second.canUse = true;
				it->second.remainigTime = 0.0f;
			}
			else
				it->second.remainigTime -= Coroutine::GetElapsedTime();
		}
	}
}
