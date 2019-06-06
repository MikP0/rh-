#pragma once

#include "pch.h"
#include "MeshSK.h"
#include "AnimationPlayer.h"
#include "EffectSK.h"
#include "MyGame.h"
#include "ModelSK.h"
#include "Utility.h"
#include "AnimationClip.h"
#include "SkinnedModelMaterial.h"
#include "ShadowModelMaterial.h"
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
	ModelSkinned(ID3D11Device1* dev, const std::string& filename, ID3D11DeviceContext1* con);
	~ModelSkinned();

	void DrawModel(ID3D11DeviceContext* deviceContext, const CommonStates& states, DirectX::XMMATRIX world, DirectX::SimpleMath::Matrix viewMat, DirectX::SimpleMath::Matrix projMat, bool wireframe = false, bool alpha = false);

	AnimationPlayer* GetAnimatorPlayer();
	bool GetInMove();
	void SetInMove(bool set);

	bool AddAnimationClip(std::string path, std::string clipName);

	DirectX::SimpleMath::Matrix character_world;

	std::string currentAnimation;
	bool playingAnimation;

	void SetCurrentAnimation(std::string animName);

	bool isHitted;
	bool isHealed;
	bool isVisible;

	bool drawToShadows;

private:
	SkinnedModelMaterial* mMaterial;
	ShadowModelMaterial* mShadowMaterial;

	EffectSK* mEffect;
	MyGame myGameTemp;
	ModelSK* mSkinnedModel;
	DirectX::PackedVector::XMCOLOR mAmbientColor;
	ID3D11Device1* DDevice;
	ID3D11DeviceContext1* DDeviceContext;
	bool inMove;
	AnimationPlayer* mAnimationPlayer;
	std::vector<ID3D11Buffer*> mVertexBuffers;
	std::vector<ID3D11Buffer*> mShadowsVertexBuffers;
	std::vector<ID3D11Buffer*> mIndexBuffers;
	std::vector<UINT> mIndexCounts;
	std::vector<ID3D11ShaderResourceView*> mColorTextures;

	void PrepareForRendering(ID3D11DeviceContext* deviceContext, const CommonStates& states, bool alpha, bool wireframe);
};