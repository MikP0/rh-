#include "pch.h"
#include "ModelSkinned.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ModelSkinned::ModelSkinned(ID3D11Device1* dev, const std::string& filename, ID3D11DeviceContext1* con)
{
	mAmbientColor.r = 1.0f;
	mAmbientColor.g = 1.0f;
	mAmbientColor.b = 1.0f;
	mAmbientColor.a = 1.0f;

	character_world = DirectX::SimpleMath::Matrix::Identity;
	DDevice = dev;
	DDeviceContext = con;
	myGameTemp.mDirect3DDevice = dev;
	myGameTemp.mDirect3DDeviceContext = con;

	mSkinnedModel = new ModelSK(myGameTemp, filename, true);

	mEffect = new EffectSK(myGameTemp);
	mEffect->LoadCompiledEffect(L"Content\\Shaders\\SkinnedModel.cso");
	mMaterial = new SkinnedModelMaterial();
	mMaterial->Initialize(*mEffect);

	mShadowMaterial = new ShadowModelMaterial();
	mShadowMaterial->Initialize(*mEffect);

	mVertexBuffers.clear();
	mIndexBuffers.clear();
	mVertexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexBuffers.resize(mSkinnedModel->Meshes().size());
	mShadowsVertexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexCounts.resize(mSkinnedModel->Meshes().size());
	mColorTextures.resize(mSkinnedModel->Meshes().size());


	for (UINT i = 0; i < mSkinnedModel->Meshes().size(); i++)
	{
		MeshSK* mesh = mSkinnedModel->Meshes().at(i);

		ID3D11Buffer* vertexBuffer = nullptr;
		mMaterial->CreateVertexBuffer(DDevice, *mesh, &vertexBuffer);
		mVertexBuffers[i] = vertexBuffer;

		ID3D11Buffer* shadowVertexBuffer = nullptr;
		mShadowMaterial->CreateVertexBuffer(DDevice, *mesh, &shadowVertexBuffer);
		mShadowsVertexBuffers[i] = shadowVertexBuffer;

		ID3D11Buffer* indexBuffer = nullptr;
		mesh->CreateIndexBuffer(&indexBuffer);
		mIndexBuffers[i] = indexBuffer;

		mIndexCounts[i] = mesh->Indices().size();

		ID3D11ShaderResourceView* colorTexture = nullptr;
		ModelMaterial* material = mesh->GetMaterial();

		if (material != nullptr && material->Textures().find(TextureTypeDifffuse) != material->Textures().cend())
		{
			std::vector<std::wstring>* diffuseTextures = material->Textures().at(TextureTypeDifffuse);
			std::wstring filename = PathFindFileName(diffuseTextures->at(0).c_str());

			std::wostringstream textureName;
			textureName << L"Content\\Models\\" << filename.substr(0, filename.length() - 4) << L".png";
			HRESULT hr = DirectX::CreateWICTextureFromFile(DDevice, DDeviceContext, textureName.str().c_str(), nullptr, &colorTexture);
			if (FAILED(hr))
			{
				throw GameException("CreateWICTextureFromFile() failed.", hr);
			}
		}
		mColorTextures[i] = colorTexture;
		//mColorTextures[i] = nullptr;
	}

	mAnimationPlayer = new AnimationPlayer(myGameTemp, *mSkinnedModel, false);

	mAnimationPlayer->SetInterpolationEnabled(!mAnimationPlayer->InterpolationEnabled());
	inMove = false;

	currentAnimation = "Idle";
	playingAnimation = true;

	isHitted = false;
	isHealed = false;
	isVisible = true;
	drawToShadows = false;
}

ModelSkinned::~ModelSkinned()
{
}

void ModelSkinned::PrepareForRendering(ID3D11DeviceContext* deviceContext, const CommonStates& states, bool alpha, bool wireframe)
{
	assert(deviceContext != nullptr);

	// Set the blend and depth stencil state.
	ID3D11BlendState* blendState;
	ID3D11DepthStencilState* depthStencilState;

	bool pmalpha = true;
	bool ccw = true;


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
		states.LinearWrap(),
	};

	deviceContext->PSSetSamplers(0, 2, samplers);
}

void ModelSkinned::DrawModel(ID3D11DeviceContext* deviceContext, const CommonStates& states, DirectX::XMMATRIX world, DirectX::SimpleMath::Matrix viewMat, DirectX::SimpleMath::Matrix projMat, bool wireframe, bool alpha)
{
	if (!drawToShadows)
	{
		character_world = world;

		PrepareForRendering(deviceContext, states, alpha, wireframe);

		Pass* pass = mMaterial->CurrentTechnique()->Passes().at(0);
		ID3D11InputLayout* inputLayout = mMaterial->InputLayouts().at(pass);
		deviceContext->IASetInputLayout(inputLayout);

		DirectX::XMMATRIX wvp = character_world * viewMat * projMat;

		DirectX::XMVECTOR ambientColor = XMLoadColor(&mAmbientColor);

		DirectX::XMVECTOR mHitted;

		if (isHitted)
		{
			mHitted = { 1.0f, 0.0, 0.0, 0.0f };
		}
		else if (isHealed)
		{
			mHitted = { 0.5f, 0.0, 0.0, 0.0f };
		}
		else
		{
			mHitted = { 0.0f, 0.0, 0.0, 0.0f };
		}

		UINT stride = mMaterial->VertexSize();
		UINT offset = 0;

		for (UINT i = 0; i < mVertexBuffers.size(); i++)
		{
			ID3D11Buffer* vertexBuffer = mVertexBuffers[i];
			ID3D11Buffer* indexBuffer = mIndexBuffers[i];
			UINT indexCount = mIndexCounts[i];
			ID3D11ShaderResourceView* colorTexture = mColorTextures[i];

			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			mMaterial->WorldViewProjection() << wvp;
			mMaterial->World() << character_world;
			mMaterial->AmbientColor() << ambientColor;
			mMaterial->ColorTexture() << colorTexture;
			mMaterial->BoneTransforms() << mAnimationPlayer->BoneTransforms();
			mMaterial->View() << viewMat;
			mMaterial->Projection() << projMat;
			mMaterial->Hitted() << mHitted;

			pass->Apply(0, deviceContext);

			deviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
	else
	{
		character_world = world;

		PrepareForRendering(deviceContext, states, alpha, wireframe);

		Pass* pass = mShadowMaterial->CurrentTechnique()->Passes().at(0);
		ID3D11InputLayout* inputLayout = mShadowMaterial->InputLayouts().at(pass);
		deviceContext->IASetInputLayout(inputLayout);

		DirectX::XMMATRIX wvp = character_world * viewMat * projMat;

		UINT stride = mShadowMaterial->VertexSize();
		UINT offset = 0;

		for (UINT i = 0; i < mShadowsVertexBuffers.size(); i++)
		{
			ID3D11Buffer* vertexBuffer = mShadowsVertexBuffers[i];
			ID3D11Buffer* indexBuffer = mIndexBuffers[i];
			UINT indexCount = mIndexCounts[i];
			ID3D11ShaderResourceView* colorTexture = mColorTextures[i];

			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			mShadowMaterial->WorldViewProjection() << wvp;
			mShadowMaterial->BoneTransforms() << mAnimationPlayer->BoneTransforms();

			pass->Apply(0, deviceContext);

			deviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}

AnimationPlayer * ModelSkinned::GetAnimatorPlayer()
{
	return mAnimationPlayer;
}

bool ModelSkinned::GetInMove()
{
	return inMove;
}

void ModelSkinned::SetInMove(bool set)
{
	inMove = set;
}

bool ModelSkinned::AddAnimationClip(std::string path, std::string clipName)
{
	Assimp::Importer importer;
	UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;

	const aiScene* scene = importer.ReadFile(path, flags);
	if (scene == nullptr)
	{
		throw GameException(importer.GetErrorString());
	}

	if (scene->HasAnimations())
	{
		AnimationClip* animationClip = new AnimationClip(*mSkinnedModel, *(scene->mAnimations[0]));
		if (!(mAnimationPlayer->AddAnimationClip(animationClip, clipName)))
			return false;
	}

	return true;
}

void ModelSkinned::SetCurrentAnimation(std::string animName)
{
	currentAnimation = animName;
	playingAnimation = true;
}
