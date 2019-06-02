#pragma once

#include "pch.h"

struct aiNodeAnim;

class ModelSK;
class Bone;
class Keyframe;

class BoneAnimation
{
	friend class AnimationClip;

public:
	~BoneAnimation();

	Bone& GetBone();
	const std::vector<Keyframe*> Keyframes() const;

	UINT GetTransform(float time, DirectX::XMFLOAT4X4& transform) const;
	void GetTransformAtKeyframe(UINT keyframeIndex, DirectX::XMFLOAT4X4& transform) const;
	void GetInteropolatedTransform(float time, DirectX::XMFLOAT4X4& transform) const;

//private:
	BoneAnimation(ModelSK& model, aiNodeAnim& nodeAnim);

	BoneAnimation();
	BoneAnimation(const BoneAnimation& rhs);
	BoneAnimation& operator=(const BoneAnimation& rhs);

	UINT FindKeyframeIndex(float time) const;

	ModelSK* mModel;
	Bone* mBone;
	std::vector<Keyframe*> mKeyframes;
};
