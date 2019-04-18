#pragma once
#include "pch.h"
#include "Pass.h"
#include <map>


class MyGame;
class EffectSK;

class Technique
{
public:
	Technique(MyGame& game, EffectSK& effect, ID3DX11EffectTechnique* technique);
	~Technique();

	EffectSK& GetEffect();
	ID3DX11EffectTechnique* GetTechnique() const;
	const D3DX11_TECHNIQUE_DESC& TechniqueDesc() const;
	const std::string& Name() const;
	const std::vector<Pass*>& Passes() const;
	const std::map<std::string, Pass*>& PassesByName() const;

private:
	Technique(const Technique& rhs);
	Technique& operator=(const Technique& rhs);

	EffectSK& mEffect;
	ID3DX11EffectTechnique* mTechnique;
	D3DX11_TECHNIQUE_DESC mTechniqueDesc;
	std::string mName;
	std::vector<Pass*> mPasses;
	std::map<std::string, Pass*> mPassesByName;
};