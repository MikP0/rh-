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

	remGameTime = 0.0f;


	lerpValue = 0.2f;


	myRemTransforms.clear();
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


	//float lerpValue = 0.1f;
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
	if (blendingisdone)
	{
		return mFinalTransforms;
	}
	else
	{
		tempTransforms.clear();

		for (int i = 0; i < mFinalTransforms.size(); i++)
		{
			/*DirectX::XMMATRIX t1 = DirectX::XMLoadFloat4x4(&mFinalTransforms.at(i));
			DirectX::XMMATRIX t2 = DirectX::XMLoadFloat4x4(&mblendTransforms.at(i));
			DirectX::XMMATRIX res = DirectX::XMMatrixMultiply(t1, t2);

			DirectX::XMStoreFloat4x4(&mFinalTransforms.at(i), res);*/

			//mFinalTransforms.at(i) = mFinalTransforms.at(i).
			
			
			//mFinalTransforms[i] = (multiIT(mFinalTransforms.at(i), mblendTransforms.at(i)));

			//mFinalTransforms.at(i) = multiIT(mFinalTransforms.at(i), mblendTransforms.at(i));


			DirectX::XMFLOAT4X4 res = (multiIT(mFinalTransforms.at(i), mblendTransforms.at(i)));

			tempTransforms.push_back(res);
		}

		//myRemTransforms = tempTransforms;

		if (lerpValue >= 0.95)
		{
			lerpValue = 0.2f;
			blendingisdone = true;
		}
		else
		{
			lerpValue += 0.035f;
		}

 		return tempTransforms;
 		//return myRemTransforms;
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
	if (mCurrentClip != nullptr)
	{
		if (mCurrentClip->Name() != clipName)
		{
			for (int i = 0; i < animationClips.size(); i++)
			{
				if (animationClips[i]->Name() == clipName)
				{
					if (blendingisdone)
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

void AnimationPlayer::resetBlend()
{
	blendingisdone = true;
	mIsBlended = false;
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

	//myRemTransforms = mblendTransforms;

	
	//mIsBlended = false;
	blendingisdone = false;
	tempTransforms.clear();

	mCurrentClip = &clip;
	mCurrentTime = 0.0f;
	mCurrentKeyframe = 0;
	mIsPlayingClip = true;

	
	//mCurrentClip->mTicksPerSecond += 60.f;

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
	remGameTime = gameTime;

	if (blendingisdone)
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


			/*if ((!blendingisdone) && ((mCurrentTime > 2.0f) && (mCurrentTime < 5.0f)))
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
			}*/

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