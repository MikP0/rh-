#include "pch.h"
#include "Blockade.h"


Blockade::Blockade(vector<string> skillsNames, vector<bool> blockadeStates)
{
	if (skillsNames.size() == blockadeStates.size())
	{
		for (int i = 0; i < skillsNames.size(); i++)
		{
			blocades[skillsNames[i]] = blockadeStates[i];
		}
	}
}


Blockade::~Blockade()
{
}

void Blockade::UnlockSkill(string skillName)
{
	blocades[skillName] = false;
}

void Blockade::LockSkill(string skillName)
{
	blocades[skillName] = true;
}

bool Blockade::IsSkillBlocked(string skillName)
{
	return blocades[skillName];
}
