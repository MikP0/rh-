#pragma once
#include <vector>
#include <map>

using namespace std;


class Blockade
{
public:
	Blockade(vector<string> skillsNames, vector<bool> blockadeStates);
	~Blockade();

	void UnlockSkill(string skillName);
	void LockSkill(string skillName);
	bool IsSkillBlocked(string skillName);

	map<string, bool> blocades;
};

