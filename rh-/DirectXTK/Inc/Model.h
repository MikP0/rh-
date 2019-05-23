//--------------------------------------------------------------------------------------
// File: Model.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <memory>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include <stdint.h>

#include <wrl\client.h>

#include <map>

namespace DirectX
{
	class IEffect;
	class IEffectFactory;
	class CommonStates;
	class ModelMesh;

	//----------------------------------------------------------------------------------
	// Each mesh part is a submesh with a single effect
	class ModelMeshPart
	{
	public:
		ModelMeshPart();
		virtual ~ModelMeshPart();

		uint32_t                                                indexCount;
		uint32_t                                                startIndex;
		uint32_t                                                vertexOffset;
		uint32_t                                                vertexStride;
		D3D_PRIMITIVE_TOPOLOGY                                  primitiveType;
		DXGI_FORMAT                                             indexFormat;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>               inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>                    indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>                    vertexBuffer;
		std::shared_ptr<IEffect>                                effect;
		std::shared_ptr<std::vector<D3D11_INPUT_ELEMENT_DESC>>  vbDecl;
		bool                                                    isAlpha;

		typedef std::vector<std::unique_ptr<ModelMeshPart>> Collection;

		// Draw mesh part with custom effect
		void __cdecl Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ IEffect* ieffect, _In_ ID3D11InputLayout* iinputLayout,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;

		// Create input layout for drawing with a custom effect.
		void __cdecl CreateInputLayout(_In_ ID3D11Device* d3dDevice, _In_ IEffect* ieffect, _Outptr_ ID3D11InputLayout** iinputLayout);

		// Change effect used by part and regenerate input layout (be sure to call Model::Modified as well)
		void __cdecl ModifyEffect(_In_ ID3D11Device* d3dDevice, _In_ std::shared_ptr<IEffect>& ieffect, bool isalpha = false);
	};


	//----------------------------------------------------------------------------------
	// A mesh consists of one or more model mesh parts
	class ModelMesh
	{
	public:
		ModelMesh();
		virtual ~ModelMesh();

		BoundingSphere              boundingSphere;
		BoundingBox                 boundingBox;
		ModelMeshPart::Collection   meshParts;
		std::wstring                name;
		bool                        ccw;
		bool                        pmalpha;
		UINT						NumFrameInfluences;
		std::vector<UINT>			frameInfluences;

		typedef std::vector<std::shared_ptr<ModelMesh>> Collection;

		// Setup states for drawing mesh
		void __cdecl PrepareForRendering(_In_ ID3D11DeviceContext* deviceContext, CommonStates& states, bool alpha = false, bool wireframe = false) const;

		// Draw the mesh
		void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext, FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection,
			bool alpha = false,
			_In_opt_ std::function<void __cdecl(IEffect* pEffect)> setCustomState = nullptr) const;

		//
		//	add the bone information for mesh
		//
	public:
		struct Bone
		{
			Bone(const wchar_t* name, INT  parentIndex, DirectX::XMFLOAT4X4 invBindPos, DirectX::XMFLOAT4X4 bindPos, DirectX::XMFLOAT4X4 localTransform) :
				Name(name), ParentIndex(parentIndex), InvBindPos(invBindPos), BindPos(bindPos), LocalTransform(localTransform)
			{

			}

			std::wstring Name;
			INT ParentIndex;
			DirectX::XMFLOAT4X4 InvBindPos;
			DirectX::XMFLOAT4X4 BindPos;
			DirectX::XMFLOAT4X4 LocalTransform;
		};

		typedef std::vector<Bone> BoneArray;

		struct Keyframe
		{
			Keyframe() : BoneIndex(0), Time(0.0f) {}

			UINT BoneIndex;
			float Time;
			DirectX::XMFLOAT4X4 Transform;
		};

		typedef std::vector<Keyframe> KeyframeArray;

		struct AnimClip
		{
			float StartTime;
			float EndTime;
			KeyframeArray Keyframes;
		};

		typedef std::map<const std::wstring, AnimClip> AnimationClipMap;
		typedef std::vector<DirectX::XMFLOAT4X4> AnimationTransform;		//The array of matrix after transforming bone anination

	public:
		BoneArray					m_bones;
		AnimationClipMap		m_animationClips;

		//using for anination calculation  
		AnimationTransform		m_animationTransformed;			//更新动画信息后的骨骼矩阵数组
		double							m_animTime;
		std::vector<bool>			m_isTransformCombined;

		void UpdateAnimation(double timeDelta, std::wstring animName);

		//void TransformBindPos();
	private:
		void CombineTransform(UINT boneIndex, ModelMesh::BoneArray const & bones, std::vector<DirectX::XMFLOAT4X4>& boneWorldTransforms);

	};

	//----------------------------------------------------------------------------------
	// A Fram consists of one or more child frame 
	class ModelFrame
	{
	public:
		std::wstring            name;
		UINT Mesh;
		UINT ParentFrame;
		UINT ChildFrame;
		UINT SiblingFrame;
		DirectX::XMFLOAT4X4 Matrix;
		XMFLOAT4X4	mBindPoseFrameMatrix;
		XMFLOAT4X4	mTransformedPoseFrameMatrix;

		UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame

		typedef std::vector<std::shared_ptr<ModelFrame>> Collection;

		// Draw the frame
		void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext, FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection,
			bool alpha = false, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;

	};

	enum FRAME_TRANSFORM_TYPE
	{
		FTT_RELATIVE = 0,
		FTT_ABSOLUTE,		//This is not currently used but is here to support absolute transformations in the future
	};

	//----------------------------------------------------------------------------------
	// A Fram consists of one or more child frame 
	class AnimationFrameData
	{
	public:
		DirectX::XMFLOAT3 Translation;
		DirectX::XMFLOAT4 Orientation;
		DirectX::XMFLOAT3 Scaling;

		//typedef std::vector<std::shared_ptr<AnimationFrameData>> Collection;
		typedef std::vector<AnimationFrameData> Collection;
	};


	//----------------------------------------------------------------------------------
	// A Fram consists of one or more child frame 
	class AnimationFrame
	{
	public:
		std::wstring frameName;
		AnimationFrameData::Collection animFrameDatas;

		typedef std::vector<std::shared_ptr<AnimationFrame>> Collection;
	};

	//----------------------------------------------------------------------------------
	// A Fram consists of one or more child frame 
	class Animation
	{
	public:
		Animation() : m_totalTime(0)
		{
		}
		//std::wstring							name;
		UINT									animFPS;
		UINT									numAnimKeys;
		UINT									numBeginKey;
		AnimationFrame::Collection	animFrames;
		FRAME_TRANSFORM_TYPE	frameTransformType;

		typedef std::map<std::wstring, std::shared_ptr<Animation>> Collection;

		//Set the begin time of animation
		void SetTotalTime(double time);

		UINT							GetAnimationKeyFromTime(double fTime);
		void								TriggerEvent(UINT keyFrame);

		typedef std::function<void(UINT)> AnimationEvent;
		bool CreateAnimationEvent(UINT keyFrame, AnimationEvent event);
	private:
		//std::shared_ptr<ModelFrame::Collection>		modelFrames;

		std::map<UINT, AnimationEvent> m_events;
		double								m_totalTime;
	};

	//----------------------------------------------------------------------------------
	// A model consists of one or more meshes
	class Model
	{
	public:
		Model() { currentAnim = (UINT)-1; }

		ModelMesh::Collection		meshes;
		ModelFrame::Collection	frames;
		Animation::Collection		animations;
		std::wstring						m_name;
		typedef std::vector<std::shared_ptr<Model>> Collection;

		std::shared_ptr<DirectX::Animation> GetAnimation(const wchar_t * animName);
		void InsertAnimation(const std::wstring & name, std::shared_ptr<DirectX::Animation>);

		// Draw all the meshes in the model
		void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext, CommonStates& states, FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection,
			bool wireframe = false, _In_opt_ std::function<void __cdecl()> setCustomState = nullptr) const;


		//Frame manipulation
		bool	LoadAnimation(const wchar_t* szFileName);						// funciton implementation is in ModelLoadSDKMESH.cpp

		void		XM_CALLCONV TransformBindPose(CXMMATRIX World);
		void		XM_CALLCONV TransformMesh(const std::wstring & animName, double fTime);

		// Notify model that effects, parts list, or mesh list has changed
		void __cdecl Modified() { mEffectCache.clear(); }

		// Update all effects used by the model
		void __cdecl UpdateEffects(_In_ std::function<void __cdecl(IEffect*)> setEffect);

		//	Update animation 
		void UpdateAnimation(double timeDelta, std::wstring animName);

		//	Draw Skeleton
		void DrawSkeleton(double timeDelta, std::wstring animName);

		//
		// Loads a model from a Visual Studio Starter Kit .CMO file
		static std::unique_ptr<Model> __cdecl CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, size_t dataSize,
			_In_ IEffectFactory& fxFactory, bool ccw = true, bool pmalpha = false);
		static std::unique_ptr<Model> __cdecl CreateFromCMO(_In_ ID3D11Device* d3dDevice, _In_z_ const wchar_t* szFileName,
			_In_ IEffectFactory& fxFactory, bool ccw = true, bool pmalpha = false);

		// Loads a model from a DirectX SDK .SDKMESH file
		static std::unique_ptr<Model> __cdecl CreateFromSDKMESH(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, _In_ size_t dataSize,
			_In_ IEffectFactory& fxFactory, bool ccw = false, bool pmalpha = false);
		static std::unique_ptr<Model> __cdecl CreateFromSDKMESH(_In_ ID3D11Device* d3dDevice, _In_z_ const wchar_t* szFileName,
			_In_ IEffectFactory& fxFactory, bool ccw = false, bool pmalpha = false);

		// Loads a model from a .VBO file
		static std::unique_ptr<Model> __cdecl CreateFromVBO(_In_ ID3D11Device* d3dDevice, _In_reads_bytes_(dataSize) const uint8_t* meshData, _In_ size_t dataSize,
			_In_opt_ std::shared_ptr<IEffect> ieffect = nullptr, bool ccw = false, bool pmalpha = false);
		static std::unique_ptr<Model> __cdecl CreateFromVBO(_In_ ID3D11Device* d3dDevice, _In_z_ const wchar_t* szFileName,
			_In_opt_ std::shared_ptr<IEffect> ieffect = nullptr, bool ccw = false, bool pmalpha = false);

	protected:
		//frame manipulation
		void XM_CALLCONV	TransformBindPoseFrame(UINT iFrame, CXMMATRIX ParentWorld);
		void								TransformFrame(std::shared_ptr<DirectX::Animation>, double fTime);
		void								TransformFrameAbsolute(UINT iFrame, double fTime);

	private:
		std::set<IEffect*>		mEffectCache;
		UINT						currentAnim;
	};
}