#pragma once

#include <vector>
#include "ModelMaterial.h"
#include "pch.h"

class MyGame;
class MeshSK;
class ModelMaterial;
class AnimationClip;
class SceneNode;
class Bone;

struct aiNode;

class ModelSK
{
	friend class MeshSK;

public:
	ModelSK(MyGame& game, const std::string& filename, bool flipUVs = false);
	~ModelSK();

	MyGame& GetGame();
	bool HasMeshes() const;
	bool HasMaterials() const;
	bool HasAnimations() const;

	const std::vector<MeshSK*>& Meshes() const;
	const std::vector<ModelMaterial*>& Materials() const;
	const std::vector<AnimationClip*>& Animations() const;
	const std::map<std::string, AnimationClip*>& AnimationsbyName() const;
	const std::vector<Bone*> Bones() const;
	const std::map<std::string, UINT> BoneIndexMapping() const;
	SceneNode* RootNode();

private:
	ModelSK(const ModelSK& rhs);
	ModelSK& operator=(const ModelSK& rhs);

	SceneNode* BuildSkeleton(aiNode& node, SceneNode* parentSceneNode);
	void ValidateModel();
	void DeleteSceneNode(SceneNode* sceneNode);

	MyGame& mGame;
	std::vector<MeshSK*> mMeshes;
	std::vector<ModelMaterial*> mMaterials;
	std::vector<AnimationClip*> mAnimations;
	std::map<std::string, AnimationClip*> mAnimationsByName;
	std::vector<Bone*> mBones;
	std::map<std::string, UINT> mBoneIndexMapping;
	SceneNode* mRootNode;
};