#include "pch.h"
#include "AnimationPlayer.h"
#include "ModelSK.h"
#include "Bone.h"
#include "AnimationClip.h"
#include "BoneAnimation.h"
#include "Keyframe.h"
#include "MatrixHelper.h"

AnimationPlayer::AnimationPlayer(MyGame& game, ModelSK& model, bool interpolationEnabled)
	:
	mModel(&model), mCurrentClip(nullptr), mCurrentTime(0.0f), mCurrentKeyframe(0U), mToRootTransforms(), mFinalTransforms(),
	mInverseRootTransform(MatrixHelper::Identity), mInterpolationEnabled(interpolationEnabled), mIsPlayingClip(false), mIsClipLooped(true)
{
	mFinalTransforms.resize(model.Bones().size());


	mIsBlended = false;
	blendingIsDone = true;

	remGameTime = 0.0f;

	lerpValue = 0.2f;
}

const ModelSK& AnimationPlayer::GetModel() const
{
	return *mModel;
}

const AnimationClip* AnimationPlayer::CurrentClip() const
{
	return mCurrentClip;
}

float AnimationPlayer::CurrentTime() const
{
	return mCurrentTime;
}

UINT AnimationPlayer::CurrentKeyframe() const
{
	return mCurrentKeyframe;
}


DirectX::XMFLOAT4X4 AnimationPlayer::multiIT(DirectX::XMFLOAT4X4 t1, DirectX::XMFLOAT4X4 t2)
{

	DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&t2);
	DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&t1);


	DirectX::XMVECTOR pos1;
	DirectX::XMVECTOR pos2;

	DirectX::XMVECTOR sca1;
	DirectX::XMVECTOR sca2;

	DirectX::XMVECTOR rot1;
	DirectX::XMVECTOR rot2;


	DirectX::XMMatrixDecompose(&sca1, &rot1, &pos1, m1);
	DirectX::XMMatrixDecompose(&sca2, &rot2, &pos2, m2);

	DirectX::XMVECTOR translation = DirectX::XMVectorLerp(pos1, pos2, lerpValue);
	DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionSlerp(rot1, rot2, lerpValue);
	DirectX::XMVECTOR scale = DirectX::XMVectorLerp(sca1, sca2, lerpValue);

	DirectX::XMFLOAT4X4 fin;
	DirectX::XMVECTOR rotationOrigin = XMLoadFloat4(&Vector4Helper::Zero);
	XMStoreFloat4x4(&fin, DirectX::XMMatrixAffineTransformation(scale, rotationOrigin, rotationQuaternion, translation));


	return fin;
}


std::vector<DirectX::XMFLOAT4X4>& AnimationPlayer::BoneTransforms()
{
	if (blendingIsDone)
	{
		return mFinalTransforms;
	}
	else
	{
		tempTransforms.clear();

		for (int i = 0; i < mFinalTransforms.size(); i++)
		{
			DirectX::XMFLOAT4X4 res = (multiIT(mFinalTransforms.at(i), mblendTransforms.at(i)));
			tempTransforms.push_back(res);
		}

		if (lerpValue >= 0.95)
		{
			lerpValue = 0.2f;
			blendingIsDone = true;
		}
		else
		{
			lerpValue += 0.035f;
		}

		return tempTransforms;
	}
}

bool AnimationPlayer::InterpolationEnabled() const
{
	return mInterpolationEnabled;
}

bool AnimationPlayer::IsPlayingClip() const
{
	return mIsPlayingClip;
}

bool AnimationPlayer::IsClipLooped() const
{
	return mIsClipLooped;
}

void AnimationPlayer::SetInterpolationEnabled(bool interpolationEnabled)
{
	mInterpolationEnabled = interpolationEnabled;
}

void AnimationPlayer::StartClip(std::string clipName)
{
	if (blendingIsDone)
	{
		if (mCurrentClip != nullptr)
		{
			if (mCurrentClip->Name() != clipName)
			{
				for (int i = 0; i < animationClips.size(); i++)
				{
					if (animationClips[i]->Name() == clipName)
					{
						if (!mIsBlended)
						{
							StartClip(*animationClips[i]);
						}
						else
						{
							if (mCurrentClip->Name() == "Idle")
								StartClip(*animationClips[i]);
							else
								StartBlendedClip(*animationClips[i]);
						}
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < animationClips.size(); i++)
			{
				if (animationClips[i]->Name() == clipName)
				{
					StartClip(*animationClips[i]);
				}
			}
		}
	}
}


void AnimationPlayer::StartClip(AnimationClip& clip)
{
	mCurrentClip = &clip;
	mCurrentTime = 0.0f;
	mCurrentKeyframe = 0;
	mIsPlayingClip = true;

	DirectX::XMMATRIX inverseRootTransform = XMMatrixInverse(&XMMatrixDeterminant(mModel->RootNode()->TransformMatrix()), mModel->RootNode()->TransformMatrix());
	XMStoreFloat4x4(&mInverseRootTransform, inverseRootTransform);
	//GetBindPose(*(mModel->RootNode()));
}

void AnimationPlayer::StartBlendedClip(AnimationClip & clip)
{
	mCurrentTime += 1.0f;
	mCurrentKeyframe += 1;

	tempTransforms = mFinalTransforms;

	Update(remGameTime);

	mblendTransforms.clear();
	mblendTransforms = mFinalTransforms;

	mFinalTransforms.clear();
	mFinalTransforms = tempTransforms;

	blendingIsDone = false;
	tempTransforms.clear();

	mCurrentClip = &clip;
	mCurrentTime = 0.0f;
	mCurrentKeyframe = 0;
	mIsPlayingClip = true;
}



void AnimationPlayer::PauseClip()
{
	mIsPlayingClip = false;
}

void AnimationPlayer::ResumeClip()
{
	if (mCurrentClip != nullptr)
	{
		mIsPlayingClip = true;
	}
}

void AnimationPlayer::Update(float gameTime)
{
	remGameTime = gameTime;

	if (blendingIsDone)
	{
		if (mIsPlayingClip)
		{
			assert(mCurrentClip != nullptr);

			mCurrentTime += static_cast<float>(gameTime) * mCurrentClip->TicksPerSecond();
			if (mCurrentTime >= mCurrentClip->Duration())
			{
				if (mIsClipLooped)
				{
					mCurrentTime = 0.0f;
				}
				else
				{
					mIsPlayingClip = false;
					return;
				}
			}


			if (mInterpolationEnabled)
			{
				GetInterpolatedPose(mCurrentTime, *(mModel->RootNode()));
			}
			else
			{
				GetPose(mCurrentTime, *(mModel->RootNode()));
			}
		}
	}
}

void AnimationPlayer::SetBlending(bool isBlended)
{
	mIsBlended = isBlended;
}

void AnimationPlayer::SetCurrentKeyFrame(UINT keyframe)
{
	mCurrentKeyframe = keyframe;
	GetPoseAtKeyframe(mCurrentKeyframe, *(mModel->RootNode()));
}

bool AnimationPlayer::AddAnimationClip(AnimationClip * clip, std::string clipName)
{
	bool isNameInVector = false;

	for (int i = 0; i < animationClips.size(); i++)
	{
		if (animationClips[i]->Name() == clipName)
			isNameInVector = true;
	}

	if (!isNameInVector)
	{
		animationClips.push_back(clip);
		clip->SetName(clipName);
	}
	else
	{
		return false;
	}

	return true;
}

void AnimationPlayer::GetBindPoseBottomUp(SceneNode& sceneNode)
{
	DirectX::XMMATRIX toRootTransform = sceneNode.TransformMatrix();

	SceneNode* parentNode = sceneNode.Parent();
	while (parentNode != nullptr)
	{
		toRootTransform = toRootTransform * parentNode->TransformMatrix();
		parentNode = parentNode->Parent();
	}

	Bone* bone = sceneNode.As<Bone>();
	if (bone != nullptr)
	{
		XMStoreFloat4x4(&(mFinalTransforms[bone->Index()]), bone->OffsetTransformMatrix() *  toRootTransform * XMLoadFloat4x4(&mInverseRootTransform));
	}

	for (SceneNode* childNode : sceneNode.Children())
	{
		GetBindPoseBottomUp(*childNode);
	}
}

void AnimationPlayer::GetBindPose(SceneNode& sceneNode)
{
	DirectX::XMMATRIX toParentTransform = sceneNode.TransformMatrix();
	DirectX::XMMATRIX toRootTransform = (sceneNode.Parent() != nullptr ? toParentTransform * XMLoadFloat4x4(&(mToRootTransforms.at(sceneNode.Parent()))) : toParentTransform);
	XMStoreFloat4x4(&(mToRootTransforms[&sceneNode]), toRootTransform);

	Bone* bone = sceneNode.As<Bone>();
	if (bone != nullptr)
	{
		XMStoreFloat4x4(&(mFinalTransforms[bone->Index()]), bone->OffsetTransformMatrix() * toRootTransform * XMLoadFloat4x4(&mInverseRootTransform));
	}

	for (SceneNode* childNode : sceneNode.Children())
	{
		GetBindPose(*childNode);
	}
}

void AnimationPlayer::GetPose(float time, SceneNode& sceneNode)
{
	DirectX::XMFLOAT4X4 toParentTransform;
	Bone* bone = sceneNode.As<Bone>();
	if (bone != nullptr)
	{
		mCurrentKeyframe = mCurrentClip->GetTransform(time, *bone, toParentTransform);
	}
	else
	{
		toParentTransform = sceneNode.Transform();
	}

	DirectX::XMMATRIX toRootTransform = (sceneNode.Parent() != nullptr ? XMLoadFloat4x4(&toParentTransform) * XMLoadFloat4x4(&(mToRootTransforms.at(sceneNode.Parent()))) : XMLoadFloat4x4(&toParentTransform));
	XMStoreFloat4x4(&(mToRootTransforms[&sceneNode]), toRootTransform);

	if (bone != nullptr)
	{
		XMStoreFloat4x4(&(mFinalTransforms[bone->Index()]), bone->OffsetTransformMatrix() * toRootTransform * XMLoadFloat4x4(&mInverseRootTransform));
	}

	for (SceneNode* childNode : sceneNode.Children())
	{
		GetPose(time, *childNode);
	}
}

void AnimationPlayer::GetPoseAtKeyframe(UINT keyframe, SceneNode& sceneNode)
{
	DirectX::XMFLOAT4X4 toParentTransform;
	Bone* bone = sceneNode.As<Bone>();
	if (bone != nullptr)
	{
		mCurrentClip->GetTransformAtKeyframe(keyframe, *bone, toParentTransform);
	}
	else
	{
		toParentTransform = sceneNode.Transform();
	}

	DirectX::XMMATRIX toRootTransform = (sceneNode.Parent() != nullptr ? XMLoadFloat4x4(&toParentTransform) * XMLoadFloat4x4(&(mToRootTransforms.at(sceneNode.Parent()))) : XMLoadFloat4x4(&toParentTransform));
	XMStoreFloat4x4(&(mToRootTransforms[&sceneNode]), toRootTransform);

	if (bone != nullptr)
	{
		XMStoreFloat4x4(&(mFinalTransforms[bone->Index()]), bone->OffsetTransformMatrix() * toRootTransform * XMLoadFloat4x4(&mInverseRootTransform));
	}

	for (SceneNode* childNode : sceneNode.Children())
	{
		GetPoseAtKeyframe(keyframe, *childNode);
	}
}

void AnimationPlayer::GetInterpolatedPose(float time, SceneNode& sceneNode)
{
	DirectX::XMFLOAT4X4 toParentTransform;
	Bone* bone = sceneNode.As<Bone>();
	if (bone != nullptr)
	{
		mCurrentClip->GetInteropolatedTransform(time, *bone, toParentTransform);
	}
	else
	{
		toParentTransform = sceneNode.Transform();
	}

	DirectX::XMMATRIX toRootTransform = (sceneNode.Parent() != nullptr ? XMLoadFloat4x4(&toParentTransform) * XMLoadFloat4x4(&(mToRootTransforms.at(sceneNode.Parent()))) : XMLoadFloat4x4(&toParentTransform));
	XMStoreFloat4x4(&(mToRootTransforms[&sceneNode]), toRootTransform);

	if (bone != nullptr)
	{
		XMStoreFloat4x4(&(mFinalTransforms[bone->Index()]), bone->OffsetTransformMatrix() * toRootTransform * XMLoadFloat4x4(&mInverseRootTransform));
	}

	for (SceneNode* childNode : sceneNode.Children())
	{
		GetInterpolatedPose(time, *childNode);
	}
}