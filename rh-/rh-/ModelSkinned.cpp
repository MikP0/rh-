#include "ModelSkinned.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const UINT DepthMapWidth = 1024U;
const UINT DepthMapHeight = 1024U;
const RECT DepthMapDestinationRectangle = { 0, 512, 256, 768 };
const float DepthBiasModulationRate = 10000;

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


	//mModelWorldMatrix = DirectX::SimpleMath::Matrix::Identity;
	//mProjectedTextureScalingMatrix = MatrixHelper::Zero;


	// Skinned and shadows
	mEffect = new EffectSK(myGameTemp);
	mEffect->LoadCompiledEffect(L"Content\\Shaders\\SkinnedModel.cso");
	mMaterial = new SkinnedModelMaterial();
	mMaterial->Initialize(*mEffect);

	// depth
	//mDepthEffect = new EffectSK(myGameTemp);
	//mDepthEffect->LoadCompiledEffect(L"Content\\Shaders\\SkinnedModel.cso");  // !!!!!!!!!111
	//mDepthMaterial = new DepthMapMaterial();
	//mDepthMaterial->Initialize(*mDepthEffect);


	mVertexBuffers.clear();
	//mDepthVertexBuffers.clear();
	mIndexBuffers.clear();
	mVertexBuffers.resize(mSkinnedModel->Meshes().size());
	//mDepthVertexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexCounts.resize(mSkinnedModel->Meshes().size());
	mColorTextures.resize(mSkinnedModel->Meshes().size());


	for (UINT i = 0; i < mSkinnedModel->Meshes().size(); i++)
	{
		MeshSK* mesh = mSkinnedModel->Meshes().at(i);

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* depthVertexBuffer = nullptr;
		mMaterial->CreateVertexBuffer(DDevice, *mesh, &vertexBuffer);

		//mDepthMaterial->CreateVertexBuffer(DDevice, *mesh, &depthVertexBuffer);

		mVertexBuffers[i] = vertexBuffer;
		//mDepthVertexBuffers[i] = depthVertexBuffer;


		ID3D11Buffer* indexBuffer = nullptr;
		mesh->CreateIndexBuffer(&indexBuffer);
		mIndexBuffers[i] = indexBuffer;

		mIndexCounts[i] = mesh->Indices().size();


		//XMStoreFloat4x4(&mModelWorldMatrix, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(0.0f, 5.0f, 2.5f));

		//mDepthMap = new DepthMap(*(&myGameTemp), DepthMapWidth, DepthMapHeight);

		//UpdateDepthBiasState();






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
	}

	mAnimationPlayer = new AnimationPlayer(myGameTemp, *mSkinnedModel, false);
	mAnimationPlayer->SetInterpolationEnabled(!mAnimationPlayer->InterpolationEnabled());
	inMove = false;
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
	character_world = world;

	PrepareForRendering(deviceContext, states, alpha, wireframe);

	Pass* pass = mMaterial->CurrentTechnique()->Passes().at(0);
	//Pass* pass2 = mMaterial->CurrentTechnique()->Passes().at(1);
	ID3D11InputLayout* inputLayout = mMaterial->InputLayouts().at(pass);
	deviceContext->IASetInputLayout(inputLayout);

	DirectX::XMMATRIX wvp = character_world * viewMat * projMat;

	DirectX::XMVECTOR ambientColor = XMLoadColor(&mAmbientColor);

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

		pass->Apply(0, deviceContext);
		//pass2->Apply(1, deviceContext);

		deviceContext->DrawIndexed(indexCount, 0, 0);
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

//void ModelSkinned::InitializeProjectedTextureScalingMatrix()
//{
//	mProjectedTextureScalingMatrix._11 = 0.5f;
//	mProjectedTextureScalingMatrix._22 = -0.5f;
//	mProjectedTextureScalingMatrix._33 = 1.0f;
//	mProjectedTextureScalingMatrix._41 = 0.5f;
//	mProjectedTextureScalingMatrix._42 = 0.5f;
//	mProjectedTextureScalingMatrix._44 = 1.0f;
//}
//
//void ModelSkinned::UpdateDepthBiasState()
//{
//	//ReleaseObject(mDepthBiasState);
//
//	D3D11_RASTERIZER_DESC rasterizerStateDesc;
//	ZeroMemory(&rasterizerStateDesc, sizeof(rasterizerStateDesc));
//	rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
//	rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
//	rasterizerStateDesc.DepthClipEnable = true;
//	rasterizerStateDesc.DepthBias = (int)mDepthBias;
//	rasterizerStateDesc.SlopeScaledDepthBias = mSlopeScaledDepthBias;
//
//	HRESULT hr = mGame->Direct3DDevice()->CreateRasterizerState(&rasterizerStateDesc, &mDepthBiasState);
//	if (FAILED(hr))
//	{
//		throw GameException("ID3D11Device::CreateRasterizerState() failed.", hr);
//	}
//}