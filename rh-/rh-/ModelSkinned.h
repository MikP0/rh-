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
#include "DepthMapMaterial.h"
#include "DepthMap.h"
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

	void InitializeProjectedTextureScalingMatrix();
	void UpdateDepthBiasState();


	DirectX::SimpleMath::Matrix character_world;

private:
	SkinnedModelMaterial* mMaterial;
	EffectSK* mEffect;

	//DepthMapMaterial* mDepthMaterial;
	//EffectSK* mDepthEffect;

	DirectX::XMFLOAT4X4 mModelWorldMatrix;
	DirectX::XMFLOAT4X4 mProjectedTextureScalingMatrix;
	//DepthMap* mDepthMap;



	MyGame myGameTemp;
	ModelSK* mSkinnedModel;
	DirectX::PackedVector::XMCOLOR mAmbientColor;
	ID3D11Device1* DDevice;
	ID3D11DeviceContext1* DDeviceContext;
	bool inMove;
	AnimationPlayer* mAnimationPlayer;
	std::vector<ID3D11Buffer*> mVertexBuffers;
	std::vector<ID3D11Buffer*> mDepthVertexBuffers;
	std::vector<ID3D11Buffer*> mIndexBuffers;
	std::vector<UINT> mIndexCounts;
	std::vector<ID3D11ShaderResourceView*> mColorTextures;

	void PrepareForRendering(ID3D11DeviceContext* deviceContext, const CommonStates& states, bool alpha, bool wireframe);
};