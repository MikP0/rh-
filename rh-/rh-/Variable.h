#pragma once
#include "pch.h"


class EffectSK;

class Variable
{
public:
	Variable(EffectSK& effect, ID3DX11EffectVariable* variable);

	EffectSK& GetEffect();
	ID3DX11EffectVariable* GetVariable() const;
	const D3DX11_EFFECT_VARIABLE_DESC& VariableDesc() const;
	ID3DX11EffectType* Type() const;
	const D3DX11_EFFECT_TYPE_DESC& TypeDesc() const;
	const std::string& Name() const;

	Variable& operator<<(DirectX::CXMMATRIX value);
	Variable& operator<<(ID3D11ShaderResourceView* value);
	Variable& operator<<(DirectX::FXMVECTOR value);
	Variable& operator<<(float value);
	Variable& operator<<(const std::vector<float>& values);
	Variable& operator<<(const std::vector<DirectX::XMFLOAT2>& values);
	Variable& operator<<(const std::vector<DirectX::XMFLOAT4X4>& values);

private:
	Variable(const Variable& rhs);
	Variable& operator=(const Variable& rhs);

	EffectSK& mEffect;
	ID3DX11EffectVariable* mVariable;
	D3DX11_EFFECT_VARIABLE_DESC mVariableDesc;
	ID3DX11EffectType* mType;
	D3DX11_EFFECT_TYPE_DESC mTypeDesc;
	std::string mName;
};
