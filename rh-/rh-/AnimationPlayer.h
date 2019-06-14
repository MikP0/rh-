#pragma once
#include "pch.h"
#include "MyGame.h"
#include "Bone.h"
#include <map>


class GameTime;
class ModelSK;
class SceneNode;
class AnimationClip;

class AnimationPlayer
{

public:
	AnimationPlayer(MyGame& game, ModelSK& model, bool interpolationEnabled = true);

	std::string myModelName = "";

	const ModelSK& GetModel() const;
	const AnimationClip* CurrentClip() const;
	float CurrentTime() const;
	UINT CurrentKeyframe() const;
	 std::vector<DirectX::XMFLOAT4X4>& BoneTransforms() ;

	bool InterpolationEnabled() const;
	bool IsPlayingClip() const;
	bool IsClipLooped() const;

	bool AddAnimationClip(AnimationClip* clip, std::string clipName);

	void SetInterpolationEnabled(bool interpolationEnabled);
	void StartClip(std::string clipName);
	
	void PauseClip();
	void ResumeClip();
	void Update(float gameTime);
	void SetCurrentKeyFrame(UINT keyframe);

	void SetBlending(bool isBlended);
	bool blendingIsDone;

	std::vector<DirectX::XMFLOAT4X4> mblendTransforms;	
	std::vector<DirectX::XMFLOAT4X4> tempTransforms;


	float remGameTime;

	float lerpValue;

	DirectX::XMFLOAT4X4 multiIT(DirectX::XMFLOAT4X4 t1, DirectX::XMFLOAT4X4 t2);

private:
	AnimationPlayer();
	AnimationPlayer(const AnimationPlayer& rhs);
	AnimationPlayer& operator=(const AnimationPlayer& rhs);

	void StartClip(AnimationClip& clip);
	void StartBlendedClip(AnimationClip& clip);

	void GetBindPose(SceneNode& sceneNode);
	void GetBindPoseBottomUp(SceneNode& sceneNode);
	void GetPose(float time, SceneNode& sceneNode);
	void GetPoseAtKeyframe(UINT keyframe, SceneNode& sceneNode);
	void GetInterpolatedPose(float time, SceneNode& sceneNode);

	ModelSK* mModel;
	AnimationClip* mCurrentClip;
	std::vector<AnimationClip*> animationClips;

	float mCurrentTime;
	UINT mCurrentKeyframe;
	std::map<SceneNode*, DirectX::XMFLOAT4X4> mToRootTransforms;
	std::vector<DirectX::XMFLOAT4X4> mFinalTransforms;
	DirectX::XMFLOAT4X4 mInverseRootTransform;
	bool mInterpolationEnabled;
	bool mIsPlayingClip;
	bool mIsClipLooped;

	bool mIsBlended;
};