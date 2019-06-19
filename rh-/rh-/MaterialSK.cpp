#include "pch.h"
#include "MaterialSK.h"
#include "GameException.h"
#include "ModelSK.h"


RTTI_DEFINITIONS(MaterialSK)

MaterialSK::MaterialSK()
	: mEffect(nullptr), mCurrentTechnique(nullptr), mDefaultTechniqueName(), mInputLayouts()
{
}

MaterialSK::MaterialSK(const std::string& defaultTechniqueName)
	: mEffect(nullptr), mCurrentTechnique(nullptr), mDefaultTechniqueName(defaultTechniqueName), mInputLayouts()
{
}

MaterialSK::~MaterialSK()
{
	for (std::pair<Pass*, ID3D11InputLayout*> inputLayout : mInputLayouts)
	{
		ReleaseObject(inputLayout.second);
	}
}

Variable* MaterialSK::operator[](const std::string& variableName)
{
	const std::map<std::string, Variable*>& variables = mEffect->VariablesByName();
	Variable* foundVariable = nullptr;

	std::map<std::string, Variable*>::const_iterator found = variables.find(variableName);
	if (found != variables.end())
	{
		foundVariable = found->second;
	}

	return foundVariable;
}

EffectSK* MaterialSK::GetEffect() const
{
	return mEffect;
}

Technique* MaterialSK::CurrentTechnique() const
{
	return mCurrentTechnique;
}

void MaterialSK::SetCurrentTechnique(Technique& currentTechnique)
{
	mCurrentTechnique = &currentTechnique;
}

const std::map<Pass*, ID3D11InputLayout*>& MaterialSK::InputLayouts() const
{
	return mInputLayouts;
}

void MaterialSK::Initialize(EffectSK& effect)
{
	for (std::pair<Pass*, ID3D11InputLayout*> inputLayout : mInputLayouts)
	{
		ReleaseObject(inputLayout.second);
	}
	mInputLayouts.clear();

	mEffect = &effect;

	Technique* defaultTechnique = nullptr;
	assert(mEffect->Techniques().size() > 0);
	if (mDefaultTechniqueName.empty() == false)
	{
		defaultTechnique = mEffect->TechniquesByName().at(mDefaultTechniqueName);
		assert(defaultTechnique != nullptr);
	}
	else
	{
		defaultTechnique = mEffect->Techniques().at(0);
	}

	SetCurrentTechnique(*defaultTechnique);
}

void MaterialSK::CreateVertexBuffer(ID3D11Device* device, const ModelSK& model, std::vector<ID3D11Buffer*>& vertexBuffers) const
{
	vertexBuffers.reserve(model.Meshes().size());
	for (MeshSK* mesh : model.Meshes())
	{
		ID3D11Buffer* vertexBuffer;
		CreateVertexBuffer(device, *mesh, &vertexBuffer);
		vertexBuffers.push_back(vertexBuffer);
	}
}

void MaterialSK::CreateInputLayout(const std::string& techniqueName, const std::string& passName, D3D11_INPUT_ELEMENT_DESC* inputElementDescriptions, UINT inputElementDescriptionCount)
{
	Technique* technique = mEffect->TechniquesByName().at(techniqueName);
	assert(technique != nullptr);

	Pass* pass = technique->PassesByName().at(passName);
	assert(pass != nullptr);

	ID3D11InputLayout* inputLayout;
	pass->CreateInputLayout(inputElementDescriptions, inputElementDescriptionCount, &inputLayout);

	mInputLayouts.insert(std::pair<Pass*, ID3D11InputLayout*>(pass, inputLayout));
}

void MaterialSK::CreateInputLayout(Pass& pass, D3D11_INPUT_ELEMENT_DESC* inputElementDescriptions, UINT inputElementDescriptionCount)
{
	ID3D11InputLayout* inputLayout;
	pass.CreateInputLayout(inputElementDescriptions, inputElementDescriptionCount, &inputLayout);

	mInputLayouts.insert(std::pair<Pass*, ID3D11InputLayout*>(&pass, inputLayout));
}