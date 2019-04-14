#pragma once

#include "pch.h"
#include "ModelSK.h"

struct aiAnimation;

class Bone;
class BoneAnimation;

class AnimationClip
{
	friend class ModelSK;

public:
	~AnimationClip();

	const std::string& Name() const;
	float Duration() const;
	float TicksPerSecond() const;
	const std::vector<BoneAnimation*>& BoneAnimations() const;
	const std::map<Bone*, BoneAnimation*>& BoneAnimationsByBone() const;
	const UINT KeyframeCount() const;

	UINT GetTransform(float time, Bone& bone, DirectX::XMFLOAT4X4& transform) const;
	void GetTransforms(float time, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const;

	void GetTransformAtKeyframe(UINT keyframe, Bone& bone, DirectX::XMFLOAT4X4& transform) const;
	void GetTransformsAtKeyframe(UINT keyframe, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const;

	void GetInteropolatedTransform(float time, Bone& bone, DirectX::XMFLOAT4X4& transform) const;
	void GetInteropolatedTransforms(float time, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const;

private:
	AnimationClip(ModelSK& model, aiAnimation& animation);

	AnimationClip();
	AnimationClip(const AnimationClip& rhs);
	AnimationClip& operator=(const AnimationClip& rhs);

	std::string mName;
	float mDuration;
	float mTicksPerSecond;
	std::vector<BoneAnimation*> mBoneAnimations;
	std::map<Bone*, BoneAnimation*> mBoneAnimationsByBone;
	UINT mKeyframeCount;
};
