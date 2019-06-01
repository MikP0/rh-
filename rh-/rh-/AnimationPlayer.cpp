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

	mIsMovingForward = true;

	mIsBlended = false;
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

const std::vector<DirectX::XMFLOAT4X4>& AnimationPlayer::BoneTransforms() const
{
	return mFinalTransforms;
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
	if (mCurrentClip != nullptr)
	{
		if (mCurrentClip->Name() != clipName)
		{
			for (int i = 0; i < animationClips.size(); i++)
			{
				if (animationClips[i]->Name() == clipName)
				{
					//StartClip(*animationClips[i]);

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

void AnimationPlayer::StartClip(AnimationClip& clip)
{
	mCurrentClip = &clip;
	mCurrentTime = 0.0f;
	mCurrentKeyframe = 0;
	mIsPlayingClip = true;

	DirectX::XMMATRIX inverseRootTransform = XMMatrixInverse(&XMMatrixDeterminant(mModel->RootNode()->TransformMatrix()), mModel->RootNode()->TransformMatrix());
	XMStoreFloat4x4(&mInverseRootTransform, inverseRootTransform);
	GetBindPose(*(mModel->RootNode()));
}


void AnimationPlayer::UpAnim()
{
	//std::string clipName = "Walk";		
	//for (int i = 0; i < animationClips.size(); i++)		
	//{		
	//	if (animationClips[i]->Name() == clipName)		
	//	{		
	//		//animationClips[i]->mDuration = 45.0f;		
	//		//animationClips[i]->mKeyframeCount = 45;		
	//		AnimationClip *newclip = new AnimationClip(*animationClips[i]);		
	//		int q = 5;		
	//		newclip->mDuration = 2;		
	//		q = 0;		
	//	}		
	//}		
}
void AnimationPlayer::StartBlendedClip(AnimationClip & clip)
{
	if (orgClip != nullptr)
		mCurrentClip = &(*orgClip);
	tempClip = new AnimationClip(clip);
	//int k = 0;		
	for (std::map<Bone*, BoneAnimation*>::const_iterator it = tempClip->mBoneAnimationsByBone.begin(); it != tempClip->mBoneAnimationsByBone.end(); ++it)
	{
		for (int k = 0; k < it->second->mKeyframes.size(); k++)
		{
			it->second->mKeyframes[k]->mTime = k;
		}
	}
	int currytime = mCurrentTime;
	std::vector <Keyframe*> keyframevec;
	keyframevec.clear();
	for (std::map<Bone*, BoneAnimation*>::const_iterator it = mCurrentClip->mBoneAnimationsByBone.begin(); it != mCurrentClip->mBoneAnimationsByBone.end(); ++it)
	{
		if (currytime < it->second->mKeyframes.size())
		{
			keyframevec.push_back(it->second->mKeyframes[currytime]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 1.0f;
		}
		else
		{
			keyframevec.push_back(it->second->mKeyframes[0]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 1.0f;
		}
		if (it->second->mKeyframes.size() > 1)
		{
			if (currytime + 1 < it->second->mKeyframes.size())
			{
				keyframevec.push_back(it->second->mKeyframes[currytime + 1]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 2.0f;
			}
			else
			{
				keyframevec.push_back(it->second->mKeyframes[1]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 2.0f;
			}
			if (currytime + 2 < it->second->mKeyframes.size())
			{
				keyframevec.push_back(it->second->mKeyframes[currytime + 2]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 3.0f;
			}
			else
			{
				keyframevec.push_back(it->second->mKeyframes[2]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 3.0f;
			}
			if (currytime + 3 < it->second->mKeyframes.size())
			{
				keyframevec.push_back(it->second->mKeyframes[currytime + 3]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 4.0f;
			}
			else
			{
				keyframevec.push_back(it->second->mKeyframes[3]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 4.0f;
			}
			if (currytime + 4 < it->second->mKeyframes.size())
			{
				keyframevec.push_back(it->second->mKeyframes[currytime + 4]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 5.0f;
			}
			else
			{
				keyframevec.push_back(it->second->mKeyframes[4]);
				keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 5.0f;
			}
		}
		else
		{
			keyframevec.push_back(it->second->mKeyframes[0]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 2.0f;
			keyframevec.push_back(it->second->mKeyframes[0]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 3.0f;
			keyframevec.push_back(it->second->mKeyframes[0]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 4.0f;
			keyframevec.push_back(it->second->mKeyframes[0]);
			keyframevec[keyframevec.size() - 1]->mTime = tempClip->mDuration + 5.0f;
		}
	}
	int i = 0;
	for (std::map<Bone*, BoneAnimation*>::const_iterator it = tempClip->mBoneAnimationsByBone.begin(); it != tempClip->mBoneAnimationsByBone.end(); ++it)
	{
		it->second->mKeyframes.push_back(keyframevec[i++]);
		it->second->mKeyframes.push_back(keyframevec[i++]);
		it->second->mKeyframes.push_back(keyframevec[i++]);
		it->second->mKeyframes.push_back(keyframevec[i++]);
		it->second->mKeyframes.push_back(keyframevec[i++]);
	}
	mIsPlayingClip = true;
	mCurrentTime = tempClip->mDuration;
	mCurrentKeyframe = tempClip->mKeyframeCount;
	tempClip->mDuration += 5.0f;
	tempClip->mKeyframeCount += 5;
	mCurrentClip = &(*tempClip);
	orgClip = &(clip);
	blendingisdone = false;

	mCurrentClip->mTicksPerSecond += 200.0f;

	//DirectX::XMMATRIX inverseRootTransform = XMMatrixInverse(&XMMatrixDeterminant(mModel->RootNode()->TransformMatrix()), mModel->RootNode()->TransformMatrix());		
	//XMStoreFloat4x4(&mInverseRootTransform, inverseRootTransform);		
	//GetBindPose(*(mModel->RootNode()));		
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
	if (mIsPlayingClip)
	{
		assert(mCurrentClip != nullptr);

		if (mIsMovingForward)
		{
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
		}
		else
		{
			mCurrentTime -= static_cast<float>(gameTime) * mCurrentClip->TicksPerSecond();
			if (mCurrentTime <= 0)
			{
				if (mIsClipLooped)
				{
					mCurrentTime = mCurrentClip->Duration();
				}
				else
				{
					mIsPlayingClip = false;
					return;
				}
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

		if ((!blendingisdone) && ((mCurrentTime > 2.0f) && (mCurrentTime < 5.0f)))
		{
			mCurrentClip->mDuration -= 6.0f;
			mCurrentClip->mKeyframeCount -= 6;
			mCurrentClip->mTicksPerSecond -= 200.0f;
			for (std::map<Bone*, BoneAnimation*>::const_iterator it = mCurrentClip->mBoneAnimationsByBone.begin(); it != mCurrentClip->mBoneAnimationsByBone.end(); ++it)
			{
				it->second->mKeyframes.pop_back();
				it->second->mKeyframes.pop_back();
				it->second->mKeyframes.pop_back();
				it->second->mKeyframes.pop_back();
				it->second->mKeyframes.pop_back();
			}
			blendingisdone = true;
		}

		if (textnow)
		{
			std::string timetext = std::to_string(mCurrentTime) + "\n";
			char text[350];
			strcpy(text, timetext.c_str());
			OutputDebugStringA(text);
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

void AnimationPlayer::SetDirection(bool isForward)
{
	mIsMovingForward = isForward;
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