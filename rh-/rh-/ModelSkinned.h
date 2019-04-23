#pragma once

#include "pch.h"
#include "MeshSK.h"
#include "AnimationPlayer.h"
#include "EffectSK.h"
#include "MyGame.h"
#include "ModelSK.h"
#include "Utility.h"
#include "SkinnedModelMaterial.h"
#include <sstream>
#include <iostream>
#include "ModelSK.h"
#include "Shlwapi.h"
#include "GameException.h"
#pragma comment(lib, "Shlwapi.lib")

class ModelSK;
class SkinnedModelMaterial;
class MeshSK;
class AnimationPlayer;

using namespace DirectX;

class ModelSkinned
{
public:
	ModelSkinned(DirectX::SimpleMath::Matrix world, ID3D11Device1* dev, ID3D11DeviceContext1* con, const std::string& filename, DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f), float scale = 1.0f);
	~ModelSkinned();

	void DrawModel(ID3D11DeviceContext* deviceContext, const CommonStates& states, bool alpha, bool wireframe, DirectX::SimpleMath::Matrix viewMat, DirectX::SimpleMath::Matrix projMat);

	AnimationPlayer* GetAnimatorPlayer();
	bool GetInMove();
	void SetInMove(bool set);

	DirectX::SimpleMath::Matrix character_world;

private:
	SkinnedModelMaterial* mMaterial;
	EffectSK* mEffect;
	MyGame myGameTemp;
	ModelSK* mSkinnedModel;
	DirectX::PackedVector::XMCOLOR mAmbientColor;
	ID3D11Device1* DDevice;
	ID3D11DeviceContext1* DDeviceContext;
	bool inMove;
	AnimationPlayer* mAnimationPlayer;
	std::vector<ID3D11Buffer*> mVertexBuffers;
	std::vector<ID3D11Buffer*> mIndexBuffers;
	std::vector<UINT> mIndexCounts;
	std::vector<ID3D11ShaderResourceView*> mColorTextures;

	void PrepareForRendering(ID3D11DeviceContext* deviceContext, const CommonStates& states, bool alpha, bool wireframe);
};