#pragma once

#include <vector>
#include <map>
#include "Coroutine.h"

using namespace std;

struct CooldownUnit
{
	float timeLimit;
	float remainigTime = 0.0f;
	bool canUse = true;
};

class Cooldown
{
public:
	Cooldown();
	Cooldown(vector<string> skillsNames, vector<float> timeLimits);
	~Cooldown();

	void StartSkillCounter(string skillName);
	bool CanUseSkill(string skillName);
	float RemainingCooldownTime(string skillName);
	void Update();

	map<string, CooldownUnit> units;
};

