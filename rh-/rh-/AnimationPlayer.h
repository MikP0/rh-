#pragma once
#include "pch.h"
#include "MyGame.h"
#include <map>


class GameTime;
class ModelSK;
class SceneNode;
class AnimationClip;

class AnimationPlayer
{

public:
	AnimationPlayer(MyGame& game, ModelSK& model, bool interpolationEnabled = true);

	const ModelSK& GetModel() const;
	const AnimationClip* CurrentClip() const;
	float CurrentTime() const;
	UINT CurrentKeyframe() const;
	const std::vector<DirectX::XMFLOAT4X4>& BoneTransforms() const;

	bool InterpolationEnabled() const;
	bool IsPlayingClip() const;
	bool IsClipLooped() const;

	void SetInterpolationEnabled(bool interpolationEnabled);

	void StartClip(AnimationClip& clip);
	void PauseClip();
	void ResumeClip();
	void Update(float gameTime);
	void SetCurrentKeyFrame(UINT keyframe);
	void SetDirection(bool isForward);

private:
	AnimationPlayer();
	AnimationPlayer(const AnimationPlayer& rhs);
	AnimationPlayer& operator=(const AnimationPlayer& rhs);

	void GetBindPose(SceneNode& sceneNode);
	void GetBindPoseBottomUp(SceneNode& sceneNode);
	void GetPose(float time, SceneNode& sceneNode);
	void GetPoseAtKeyframe(UINT keyframe, SceneNode& sceneNode);
	void GetInterpolatedPose(float time, SceneNode& sceneNode);

	ModelSK* mModel;
	AnimationClip* mCurrentClip;
	float mCurrentTime;
	UINT mCurrentKeyframe;
	std::map<SceneNode*, DirectX::XMFLOAT4X4> mToRootTransforms;
	std::vector<DirectX::XMFLOAT4X4> mFinalTransforms;
	DirectX::XMFLOAT4X4 mInverseRootTransform;
	bool mInterpolationEnabled;
	bool mIsPlayingClip;
	bool mIsClipLooped;

	bool mIsMovingForward;
};