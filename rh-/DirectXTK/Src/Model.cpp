//--------------------------------------------------------------------------------------
// File: Model.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "Model.h"

#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "PlatformHelpers.h"

using namespace DirectX;
using namespace std;

#ifndef _CPPRTTI 
#error Model requires RTTI
#endif


const uint32_t INVALID_FRAME = uint32_t(-1);
const uint32_t INVALID_ANIMATION_DATA = uint32_t(-1);

//--------------------------------------------------------------------------------------
// ModelMeshPart
//--------------------------------------------------------------------------------------

ModelMeshPart::ModelMeshPart() :
	indexCount(0),
	startIndex(0),
	vertexOffset(0),
	vertexStride(0),
	primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	indexFormat(DXGI_FORMAT_R16_UINT),
	isAlpha(false)
{
}


ModelMeshPart::~ModelMeshPart()
{
}


_Use_decl_annotations_
void ModelMeshPart::Draw(ID3D11DeviceContext* deviceContext, IEffect* ieffect, ID3D11InputLayout* iinputLayout, std::function<void()> setCustomState) const
{
	deviceContext->IASetInputLayout(iinputLayout);

	auto vb = vertexBuffer.Get();
	UINT vbStride = vertexStride;
	UINT vbOffset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &vb, &vbStride, &vbOffset);

	// Note that if indexFormat is DXGI_FORMAT_R32_UINT, this model mesh part requires a Feature Level 9.2 or greater device
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), indexFormat, 0);

	assert(ieffect != 0);
	ieffect->Apply(deviceContext);

	// Hook lets the caller replace our shaders or state settings with whatever else they see fit.
	if (setCustomState)
	{
		setCustomState();
	}

	// Draw the primitive.
	deviceContext->IASetPrimitiveTopology(primitiveType);

	deviceContext->DrawIndexed(indexCount, startIndex, vertexOffset);
}


_Use_decl_annotations_
void ModelMeshPart::CreateInputLayout(ID3D11Device* d3dDevice, IEffect* ieffect, ID3D11InputLayout** iinputLayout)
{
	if (!vbDecl || vbDecl->empty())
		throw std::exception("Model mesh part missing vertex buffer input elements data");

	void const* shaderByteCode;
	size_t byteCodeLength;

	assert(ieffect != 0);
	ieffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	assert(d3dDevice != 0);

	ThrowIfFailed(
		d3dDevice->CreateInputLayout(vbDecl->data(),
			static_cast<UINT>(vbDecl->size()),
			shaderByteCode, byteCodeLength,
			iinputLayout)
	);

	_Analysis_assume_(*iinputLayout != 0);
}


_Use_decl_annotations_
void ModelMeshPart::ModifyEffect(ID3D11Device* d3dDevice, std::shared_ptr<IEffect>& ieffect, bool isalpha)
{
	if (!vbDecl || vbDecl->empty())
		throw std::exception("Model mesh part missing vertex buffer input elements data");

	assert(ieffect != 0);
	this->effect = ieffect;
	this->isAlpha = isalpha;

	void const* shaderByteCode;
	size_t byteCodeLength;

	effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	assert(d3dDevice != 0);

	ThrowIfFailed(
		d3dDevice->CreateInputLayout(vbDecl->data(),
			static_cast<UINT>(vbDecl->size()),
			shaderByteCode, byteCodeLength,
			&inputLayout)
	);
}


//--------------------------------------------------------------------------------------
// ModelMesh
//--------------------------------------------------------------------------------------

ModelMesh::ModelMesh() :
	ccw(true),
	pmalpha(true),
	NumFrameInfluences(0),
	m_animTime(0.f)	//set it to zero when changing animation 
{
}

ModelMesh::~ModelMesh()
{

}

_Use_decl_annotations_
void ModelMesh::PrepareForRendering(ID3D11DeviceContext* deviceContext, CommonStates& states, bool alpha, bool wireframe) const
{
	assert(deviceContext != 0);

	// Set the blend and depth stencil state.
	ID3D11BlendState* blendState;
	ID3D11DepthStencilState* depthStencilState;

	if (alpha)
	{
		if (pmalpha)
		{
			blendState = states.AlphaBlend();
			depthStencilState = states.DepthRead();
		}
		else
		{
			blendState = states.NonPremultiplied();
			depthStencilState = states.DepthRead();
		}
	}
	else
	{
		blendState = states.Opaque();
		depthStencilState = states.DepthDefault();
	}

	deviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(depthStencilState, 0);

	// Set the rasterizer state.
	if (wireframe)
		deviceContext->RSSetState(states.Wireframe());
	else
		deviceContext->RSSetState(ccw ? states.CullCounterClockwise() : states.CullClockwise());

	// Set sampler state.
	ID3D11SamplerState* samplers[] =
	{
		states.LinearWrap(),
		states.LinearWrap(),	//line sample state for variance shadow map 
	};

	deviceContext->PSSetSamplers(0, _countof(samplers), samplers);
}


_Use_decl_annotations_
void XM_CALLCONV ModelMesh::Draw(ID3D11DeviceContext* deviceContext,
	FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection,
	bool alpha,
	std::function<void __cdecl(IEffect* pEffect)> setCustomState) const
{
	assert(deviceContext != 0);

	for (auto it = meshParts.cbegin(); it != meshParts.cend(); ++it)
	{
		auto part = (*it).get();
		assert(part != 0);

		if (part->isAlpha != alpha)
		{
			// Skip alpha parts when drawing opaque or skip opaque parts if drawing alpha
			continue;
		}

		auto imatrices = dynamic_cast<IEffectMatrices*>(part->effect.get());
		if (imatrices)
		{
			imatrices->SetWorld(world);
			imatrices->SetView(view);
			imatrices->SetProjection(projection);
		}

		IEffectSkinning * pEffectSkinning = dynamic_cast<IEffectSkinning*>(part->effect.get());
		if (pEffectSkinning && m_animationTransformed.size())
		{
			assert(m_animationTransformed.size() <= SkinnedEffect::MaxBones);

			//update bone transforms 	
			XMMATRIX BoneTransforms[SkinnedEffect::MaxBones];
			for (UINT i = 0; i < m_animationTransformed.size() && i < SkinnedEffect::MaxBones; i++)
			{
				BoneTransforms[i] = XMLoadFloat4x4(&m_animationTransformed[i]);
			}

			pEffectSkinning->SetBoneTransforms(BoneTransforms, m_animationTransformed.size());
		}

		auto setNewState = [&]() -> void
		{
			setCustomState(part->effect.get());
		};

		part->Draw(deviceContext, part->effect.get(), part->inputLayout.Get(), setNewState);
	}
}
void ModelMesh::CombineTransform(UINT boneIndex, ModelMesh::BoneArray const & bones, std::vector<DirectX::XMFLOAT4X4>& boneWorldTransforms)
{
	auto & bone = bones[boneIndex];
	if (m_isTransformCombined[boneIndex] || bone.ParentIndex < 0 || bone.ParentIndex == static_cast<int>(boneIndex))
	{
		m_isTransformCombined[boneIndex] = true;
		return;
	}

	CombineTransform(bone.ParentIndex, bones, boneWorldTransforms);

	DirectX::XMMATRIX leftMat = XMLoadFloat4x4(&boneWorldTransforms[boneIndex]);
	DirectX::XMMATRIX rightMat = XMLoadFloat4x4(&boneWorldTransforms[bone.ParentIndex]);

	DirectX::XMMATRIX ret = leftMat * rightMat;

	XMStoreFloat4x4(&boneWorldTransforms[boneIndex], ret);

	m_isTransformCombined[boneIndex] = true;
}

//void ModelMesh::TransformBindPos()
//{
//	for (UINT i = 0; i < m_bones.size(); i++)
//	{
//		m_animationTransformed[i] = m_bones[i].BindPos;
//	}
//
//	m_isTransformCombined.assign(m_bones.size(), false);
//
//	for (UINT i = 0; i < m_bones.size(); i++)
//	{
//		CombineTransform(i, m_bones, m_animationTransformed);
//	}
//
//	for (UINT i = 0; i < m_bones.size(); i++)
//	{
//		DirectX::XMMATRIX ret = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_animationTransformed[i]));
//
//		XMStoreFloat4x4(&m_bones[i].InvBindPos, ret) ;
//	}
//}

void ModelMesh::UpdateAnimation(double timeDelta, std::wstring animName)
{
	m_animTime += timeDelta;

	if (m_bones.size() == 0)
		return;

	for (UINT i = 0; i < m_bones.size(); i++)
	{
		XMVECTOR scale, rot, trans;
		XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_bones[i].LocalTransform));

		m_animationTransformed[i] = m_bones[i].LocalTransform;
	}


	auto found = m_animationClips.find(animName);

	if (found != end(m_animationClips))
	{

		const auto & keyFrames = found->second.Keyframes;

		int startFrame = keyFrames.size() / (int)((found->second.EndTime - found->second.StartTime) * (m_animTime - found->second.StartTime));

		startFrame = (startFrame - (int)m_bones.size() * 2) > 0 ? startFrame - m_bones.size() * 2 : 0;	//œÚ«∞ÕÀ2÷°

		for (UINT i = startFrame; i < keyFrames.size(); i++)
		{
			const auto & frame = keyFrames[i];

			if (frame.Time > m_animTime)
				break;

			XMVECTOR scale, rot, trans;
			XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&frame.Transform));

			XMVECTOR	axis;
			float				angle;
			XMQuaternionToAxisAngle(&axis, &angle, rot);

			//m_animationTransformed[frame.BoneIndex] = frame.Transform;
			XMStoreFloat4x4(&m_animationTransformed[frame.BoneIndex], XMLoadFloat4x4(&frame.Transform));
		}

		// Transform to world.
		m_isTransformCombined.assign(m_bones.size(), false);

		for (UINT i = 0; i < m_bones.size(); i++)
		{
			CombineTransform(i, m_bones, m_animationTransformed);
		}

		for (UINT b = 0; b < m_bones.size(); b++)
		{
			//The matrix InvBindPos is equal to the inverse link transform of cluster in FBX SDK which is got form GetTransformLinkMatrix
			DirectX::XMMATRIX leftMat = XMLoadFloat4x4(&m_bones[b].InvBindPos);
			DirectX::XMMATRIX rightMat = XMLoadFloat4x4(&m_animationTransformed[b]);

			DirectX::XMMATRIX ret = leftMat * rightMat;

			/*XMVECTOR scale, rot, trans;
			XMMatrixDecompose(&scale, &rot, &trans, leftMat);

			XMVECTOR	axis;
			float				angle;
			XMQuaternionToAxisAngle(&axis, &angle, rot);*/

			XMStoreFloat4x4(&m_animationTransformed[b], ret);
		}

		if (m_animTime > found->second.EndTime)
		{
			m_animTime = found->second.StartTime + (m_animTime - found->second.EndTime);
		}


	}


}


_Use_decl_annotations_
void XM_CALLCONV Model::Draw(ID3D11DeviceContext* deviceContext, CommonStates& states,
	FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection,
	bool wireframe, std::function<void()> setCustomState) const
{
	assert(deviceContext != 0);

	// Draw opaque parts
	for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);

		mesh->PrepareForRendering(deviceContext, states, false, wireframe);

		//update bone infulences 
		XMMATRIX mFrameInfluences[SkinnedEffect::MaxBones];
		for (UINT i = 0; i < mesh->NumFrameInfluences && i < _countof(mFrameInfluences); i++)
		{
			UINT iFrame = mesh->frameInfluences[i];
			mFrameInfluences[i] = XMLoadFloat4x4(&frames[iFrame]->mTransformedPoseFrameMatrix);
			//frames[iFrame]->mTransformedPoseFrameMatrix;
			XMMATRIX TransformedPose = XMLoadFloat4x4(&frames[iFrame]->mTransformedPoseFrameMatrix);
			XMMATRIX bindMat = XMLoadFloat4x4(&frames[iFrame]->mBindPoseFrameMatrix);
			mFrameInfluences[i] = TransformedPose;

		}

		auto updateBones = [&](IEffect* pEffect)
		{
			if (setCustomState)
				setCustomState();

			auto pEffectSkinning = dynamic_cast<IEffectSkinning*>(pEffect);
			if (pEffectSkinning)
			{
				pEffectSkinning->SetBoneTransforms(&mFrameInfluences[0], mesh->NumFrameInfluences);
			}
		};

		mesh->Draw(deviceContext, world, view, projection, false, updateBones);
	}

	// Draw alpha parts
	for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);

		mesh->PrepareForRendering(deviceContext, states, true, wireframe);

		auto updateBones = [&](IEffect* pEffect)
		{
			if (setCustomState)
				setCustomState();

			pEffect;
		};

		mesh->Draw(deviceContext, world, view, projection, true, updateBones);
	}
}


void Model::UpdateEffects(_In_ std::function<void(IEffect*)> setEffect)
{
	if (mEffectCache.empty())
	{
		// This cache ensures we only set each effect once (could be shared)
		for (auto mit = meshes.cbegin(); mit != meshes.cend(); ++mit)
		{
			auto mesh = mit->get();
			assert(mesh != 0);

			for (auto it = mesh->meshParts.cbegin(); it != mesh->meshParts.cend(); ++it)
			{
				if ((*it)->effect != 0)
					mEffectCache.insert((*it)->effect.get());
			}
		}
	}

	assert(setEffect != 0);

	for (auto it = mEffectCache.begin(); it != mEffectCache.end(); ++it)
	{
		setEffect(*it);
	}
}

void Model::TransformBindPose(CXMMATRIX World)
{
	TransformBindPoseFrame(0, World);

	//for (auto mesh : meshes)
	//{
	//	mesh->TransformBindPos();
	//}
}

void  Model::TransformBindPoseFrame(UINT iFrame, CXMMATRIX ParentWorld)
{

	// Transform ourselves
	XMMATRIX  LocalWorld = XMLoadFloat4x4(&frames[iFrame]->Matrix);

#ifdef _DEBUG
	//XMVECTOR scale, rot, trans;
	//XMMatrixDecompose(&scale, &rot, &trans, LocalWorld);
#endif 

	//LocalWorld *= ParentWorld;
	XMStoreFloat4x4(&frames[iFrame]->mBindPoseFrameMatrix, LocalWorld);

	// Transform our siblings
	if (frames[iFrame]->SiblingFrame != INVALID_FRAME)
		TransformBindPoseFrame(frames[iFrame]->SiblingFrame, ParentWorld);

	// Transform our children
	if (frames[iFrame]->ChildFrame != INVALID_FRAME)
		TransformBindPoseFrame(frames[iFrame]->ChildFrame, LocalWorld);

}

void  Model::TransformMesh(const wstring & animName, double fTime)
{
	auto iter = animations.find(animName);

	if (iter == end(animations))
		return;

	auto anim = iter->second;

	if (FTT_RELATIVE == anim->frameTransformType)
	{
		TransformFrame(anim, fTime);

		// For each frame, move the transform to the bind pose, then
		// move it to the final position
		for (UINT i = 0; i < frames.size(); i++)
		{
			XMMATRIX BindPosMat = XMLoadFloat4x4(&frames[i]->mBindPoseFrameMatrix);
			XMMATRIX TransfromedPosMat = XMLoadFloat4x4(&frames[i]->mTransformedPoseFrameMatrix);

			//XMMATRIX mFinal = XMMatrixInverse(NULL, BindPosMat) * TransfromedPosMat;
			XMMATRIX mFinal = BindPosMat * TransfromedPosMat;

			XMStoreFloat4x4(&frames[i]->mTransformedPoseFrameMatrix, mFinal);
		}
	}
	else if (FTT_ABSOLUTE == anim->frameTransformType)
	{
		for (UINT i = 0; i < frames.size(); i++)
			TransformFrameAbsolute(i, fTime);
	}
}

UINT Animation::GetAnimationKeyFromTime(double fTime)
{
	m_totalTime += fTime;

	UINT iTick = (UINT)(animFPS * m_totalTime);

	iTick = iTick % (numAnimKeys);
	//iTick++;	

	iTick += numBeginKey;

	return iTick;
}

void	Animation::TriggerEvent(UINT keyFrame)
{
	//call back animotion event
	auto iter = m_events.find(keyFrame);
	if (iter != end(m_events))
	{
		iter->second(keyFrame);
	}
}

void Animation::SetTotalTime(double time)
{
	m_totalTime = time;
}

bool  Animation::CreateAnimationEvent(UINT keyFrame, AnimationEvent event)
{
	if (keyFrame > numAnimKeys)
		return false;

	m_events[keyFrame] = event;

	return true;
}

//
//	transform frame using loop
//
void	Model::TransformFrame(std::shared_ptr<DirectX::Animation>anim, double fTime)
{
	UINT iFrame = 0;
	XMMATRIX LocalTransform;
	XMMATRIX ParentWorld = XMMatrixIdentity();

	//	Get animation ticket
	UINT iTick = anim->GetAnimationKeyFromTime(fTime);

	//Trigger Animation Event
	anim->TriggerEvent(iTick - anim->numBeginKey);

	std::vector<bool> isTransformed(frames.size(), false);
	int index = 0;

	for (auto & modelFrame : frames)
	{
		if (modelFrame->ParentFrame != INVALID_FRAME)
		{
			assert(isTransformed[modelFrame->ParentFrame]);
			ParentWorld = XMLoadFloat4x4(&frames[modelFrame->ParentFrame]->mTransformedPoseFrameMatrix);
		}

		UINT iAnimDataIndex = modelFrame->AnimationDataIndex;

		if (INVALID_ANIMATION_DATA != iAnimDataIndex)
		{
			auto & animFrame = anim->animFrames[iAnimDataIndex];
			auto & animData = animFrame->animFrameDatas[iTick];

			//XMMATRIX mScale = XMMatrixScaling(animData.Scaling.x, animData.Scaling.y, animData.Scaling.z);

			// turn it into a matrix (Ignore scaling for now)
			XMMATRIX mTranslate = XMMatrixTranslation(animData.Translation.x, animData.Translation.y, animData.Translation.z);

			XMVECTOR quat;

			if (animData.Orientation.x == 0 && animData.Orientation.y == 0 && animData.Orientation.z == 0)
				quat = XMQuaternionIdentity();
			else
				quat = XMLoadFloat4(&animData.Orientation);

			quat = XMQuaternionNormalize(quat);

			LocalTransform = XMMatrixRotationQuaternion(quat) * mTranslate;
		}
		else
		{
			LocalTransform = XMLoadFloat4x4(&modelFrame->Matrix);
		}

		XMMATRIX localWorld = LocalTransform * ParentWorld;
		XMStoreFloat4x4(&modelFrame->mTransformedPoseFrameMatrix, localWorld);

		isTransformed[index++] = true;
	}
}

void Model::TransformFrameAbsolute(UINT iFrame, double fTime)
{
	iFrame;
	fTime;
}

void Model::UpdateAnimation(double timeDelta, std::wstring animName)
{
	for (auto & mesh : meshes)
	{
		mesh->UpdateAnimation(timeDelta, animName);
	}
}
//
//	GetAnimation
//
std::shared_ptr<DirectX::Animation>  Model::GetAnimation(const wchar_t * animName)
{
	auto iter = animations.find(animName);

	if (iter != end(animations))
		return iter->second;
	else
		return nullptr;
}

//
//		Insert a new Animation
//
void Model::InsertAnimation(const std::wstring & animName, std::shared_ptr<DirectX::Animation> animation)
{
	animations[animName] = animation;
}

//	Draw Skeleton
void Model::DrawSkeleton(double timeDelta, std::wstring animName)
{

}

