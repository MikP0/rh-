#include "pch.h"
#include "ReflectionEffect.h"


using namespace DirectX;

#define MAX_NUMBER_OF_LIGHT 25


struct POINT_LIGHT
{
	DirectX::XMFLOAT3 Position;
	float Radius;
	DirectX::XMFLOAT4 Color;
};

struct DIRECTIONAL_LIGHT
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 Direction;
	float EmptyTo16B;
};

struct SPOT_LIGHT
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 Direction;
	float OuterAngle;
	DirectX::XMFLOAT3 Position;
	float InnerAngle;
	float Radius;
	DirectX::XMFLOAT3 SpaceTo16B;
};

struct StaticConstantBuffer
{
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 EmissiveColor;
	float SpecularPower;
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 SpecularColor;

	POINT_LIGHT PointLight[MAX_NUMBER_OF_LIGHT];
	DIRECTIONAL_LIGHT DirectionalLight[MAX_NUMBER_OF_LIGHT];
	SPOT_LIGHT SpotLight[MAX_NUMBER_OF_LIGHT];

	DirectX::XMFLOAT4 NumOfLights;	//x - point, y - dir, z - spot
};

struct DynamicConstantBuffer
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 WorldViewProjection;
	DirectX::XMFLOAT3 CameraPosition;
	float IsTextured;

	DirectX::XMFLOAT4X4 WorldInverseTranspose;
};


class ReflectionEffect::Impl
{
public:

	std::string materialName;

	XMMATRIX m_world;
	XMMATRIX m_view;
	XMMATRIX m_projection;
	XMFLOAT3 m_camera;

	XMFLOAT3 diffuseColor;
	XMFLOAT3 emissiveColor;
	float alpha;

	XMVECTOR AmbientColor;
	XMVECTOR SpecularColor;
	XMVECTOR SpecularPower;
	XMVECTOR IsTextured;


	POINT_LIGHT PointLights[MAX_NUMBER_OF_LIGHT];
	DIRECTIONAL_LIGHT DirectionalLights[MAX_NUMBER_OF_LIGHT];
	SPOT_LIGHT SpotLights[MAX_NUMBER_OF_LIGHT];
	int NumOfPL = 0;
	int NumOfDL = 0;
	int NumOfSL = 0;
	XMFLOAT4 NumOfLights;

	float FillTo16 = 0.0f;
	XMFLOAT3 Fill3To16 = { 0.0f, 0.0f, 0.0f };

	bool isSent = false;

	bool m_isInit = false;
	bool m_isTextured = false;
	bool m_isCubed = false;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_staticConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMap;
	StaticConstantBuffer m_staticData;
	DynamicConstantBuffer m_dynamicData;
	std::vector<uint8_t> m_VSBytecode;


	ReflectionEffect::Impl(_In_ ID3D11Device* device);
	void Apply(_In_ ID3D11DeviceContext* deviceContext);
	void GetVertexShaderBytecode(
		_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength);

	HRESULT loadShaders(ID3D11Device* device);

	static void* operator new(size_t size)
	{
		const size_t alignment = _alignof(ReflectionEffect::Impl);
		static_assert(
			alignment > 8,
			"AllignedNew is only useful for types with > 8 byte alignment. Did you forget a _declspec(align) on TDerived?");

		void* ptr = _aligned_malloc(size, alignment);
		if (!ptr)
			throw std::bad_alloc();
		return ptr;
	}

	static void operator delete(void* ptr) { _aligned_free(ptr); }
};

ReflectionEffect::Impl::Impl(_In_ ID3D11Device* device)
{
	m_isInit = false;

	AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularPower = { 0.0 };

	m_camera.x = 0;
	m_camera.y = 0;
	m_camera.z = 0;

	IsTextured = { 0.0f };

	if (FAILED(loadShaders(device)))
	{
		return;
	}

	CD3D11_BUFFER_DESC staticBufferDesc(
		sizeof(StaticConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	if (FAILED(device->CreateBuffer(
		&staticBufferDesc, nullptr, &m_staticConstantBuffer)))
	{
		return;
	};

	CD3D11_BUFFER_DESC dynamicBufferDesc(
		sizeof(DynamicConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

	if (FAILED(device->CreateBuffer(
		&dynamicBufferDesc, nullptr, &m_dynamicConstantBuffer)))
	{
		return;
	}

	m_isInit = true;
}

void ReflectionEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	if (!m_isInit)
	{
		return;
	}

	if (!isSent)
	{
		if (!m_isTextured)
		{
			XMVECTOR DiffuseColorFin;
			XMVECTOR EmissiveColor;

			XMVECTOR diffuse = { diffuseColor.x, diffuseColor.y, diffuseColor.z };
			XMVECTOR emissive = { emissiveColor.x, emissiveColor.y, emissiveColor.z };
			XMVECTOR alphaVector = XMVectorReplicate(alpha);
			static const XMVECTORF32 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };

			EmissiveColor = XMVectorMultiply(XMVectorMultiplyAdd(AmbientColor, diffuse, emissive), alphaVector);

			DiffuseColorFin = XMVectorSelect(alphaVector, XMVectorMultiply(diffuse, alphaVector), g_XMSelect1110);

			XMStoreFloat4(&m_staticData.DiffuseColor, DiffuseColorFin);
			XMStoreFloat3(&m_staticData.EmissiveColor, EmissiveColor);
			XMStoreFloat(&m_staticData.SpecularPower, SpecularPower);
			XMStoreFloat4(&m_staticData.AmbientColor, AmbientColor);
			XMStoreFloat4(&m_staticData.SpecularColor, SpecularColor);

			for (int i = 0; i < NumOfPL; i++)
			{
				m_staticData.PointLight[i] = PointLights[i];
			}

			for (int i = 0; i < NumOfDL; i++)
			{
				m_staticData.DirectionalLight[i] = DirectionalLights[i];
			}

			for (int i = 0; i < NumOfSL; i++)
			{
				m_staticData.SpotLight[i] = SpotLights[i];
			}

			NumOfLights.x = NumOfPL;
			NumOfLights.y = NumOfDL;
			NumOfLights.z = NumOfSL;
			NumOfLights.w = 0.0f;

			m_staticData.NumOfLights = NumOfLights;
		}
		else
		{
			static const DirectX::XMVECTOR EmissiveColorTemp = { 0.0f, 0.0f, 0.0f };
			static const DirectX::XMVECTOR DiffuseColorTemp = { 0.0f, 0.0f, 0.0f, 0.0f };

			XMStoreFloat4(&m_staticData.DiffuseColor, DiffuseColorTemp);
			XMStoreFloat3(&m_staticData.EmissiveColor, EmissiveColorTemp);
			XMStoreFloat(&m_staticData.SpecularPower, SpecularPower);
			XMStoreFloat4(&m_staticData.AmbientColor, AmbientColor);
			XMStoreFloat4(&m_staticData.SpecularColor, SpecularColor);

			for (int i = 0; i < NumOfPL; i++)
			{
				m_staticData.PointLight[i] = PointLights[i];
			}

			for (int i = 0; i < NumOfDL; i++)
			{
				m_staticData.DirectionalLight[i] = DirectionalLights[i];
			}

			for (int i = 0; i < NumOfSL; i++)
			{
				m_staticData.SpotLight[i] = SpotLights[i];
			}

			IsTextured = { 1.0f };

			NumOfLights.x = NumOfPL;
			NumOfLights.y = NumOfDL;
			NumOfLights.z = NumOfSL;
			NumOfLights.w = 0.0f;

			m_staticData.NumOfLights = NumOfLights;
		}

		isSent = true;
	}

	// Update the static buffer
	deviceContext->UpdateSubresource(
		m_staticConstantBuffer.Get(), 0, NULL, &m_staticData, 0, 0);

	deviceContext->VSSetConstantBuffers(
		0, 1, m_staticConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(
		0, 1, m_staticConstantBuffer.GetAddressOf());


	XMStoreFloat4x4(&m_dynamicData.World, XMMatrixTranspose(m_world));
	XMStoreFloat4x4(&m_dynamicData.View, XMMatrixTranspose(m_view));
	XMStoreFloat4x4(&m_dynamicData.Projection, XMMatrixTranspose(m_projection));
	XMStoreFloat4x4(&m_dynamicData.WorldViewProjection, XMMatrixTranspose(XMMatrixMultiply((XMMatrixMultiply(m_world, m_view)), m_projection)));
	
	DirectX::SimpleMath::Matrix m_invert_world = m_world;

	XMStoreFloat4x4(&m_dynamicData.World, XMMatrixTranspose(m_invert_world.Invert()));


	XMVECTOR cam = { m_camera.x, m_camera.y, m_camera.z };
	XMStoreFloat3(&m_dynamicData.CameraPosition, cam);

	XMStoreFloat(&m_dynamicData.IsTextured, IsTextured);

	deviceContext->UpdateSubresource(
		m_dynamicConstantBuffer.Get(), 0, NULL, &m_dynamicData, 0, 0);

	deviceContext->VSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());


	//ID3D11ShaderResourceView* mCubeMapSRV;
	//HR(D3DX11CreateShaderResourceViewFromFile(device,
	//	cubemapFilename.c_str(), 0, 0, &mCubeMapSRV, 0));

	ID3D11ShaderResourceView* textures[1];

	/*if (m_isTextured)
	{
		textures[0] = m_texture.Get();
	}
	else
	{
		textures[0] = nullptr;
	}*/

	//if (m_isCubed)
	//{
		textures[0] = m_cubeMap.Get();
	//}
	//else
	//{
	//	textures[1] = nullptr;
	//}

	deviceContext->PSSetShaderResources(
		0, static_cast<UINT>(1), textures);

	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void ReflectionEffect::Impl::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	*pShaderByteCode = m_VSBytecode.empty() ? nullptr : &m_VSBytecode[0];
	*pByteCodeLength = m_VSBytecode.size();
}

HRESULT ReflectionEffect::Impl::loadShaders(ID3D11Device* device)
{
	try
	{
		m_VSBytecode = DX::ReadData(L"VSReflectionShader.cso");
		DX::ThrowIfFailed(device->CreateVertexShader(
			m_VSBytecode.data(),
			m_VSBytecode.size(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()));

		auto psData = DX::ReadData(L"PSReflectionShader.cso");
		DX::ThrowIfFailed(device->CreatePixelShader(
			psData.data(),
			psData.size(),
			nullptr,
			m_pixelShader.ReleaseAndGetAddressOf()));
	}
	catch (...)
	{
		return E_FAIL;
	}
	return S_OK;
}

ReflectionEffect::ReflectionEffect(_In_ ID3D11Device* device)
	: m_pImpl(new Impl(device))
{
}

ReflectionEffect::~ReflectionEffect() {}

ReflectionEffect::ReflectionEffect(ReflectionEffect&& moveFrom)
	: m_pImpl(std::move(moveFrom.m_pImpl))
{
}

ReflectionEffect& ReflectionEffect::operator=(ReflectionEffect&& moveFrom)
{
	m_pImpl = std::move(moveFrom.m_pImpl);
	return *this;
}

void ReflectionEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	m_pImpl->Apply(deviceContext);
}

void ReflectionEffect::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	m_pImpl->GetVertexShaderBytecode(pShaderByteCode, pByteCodeLength);
}

void XM_CALLCONV ReflectionEffect::SetWorld(FXMMATRIX value)
{
	m_pImpl->m_world = value;
}

void XM_CALLCONV ReflectionEffect::SetView(FXMMATRIX value)
{
	m_pImpl->m_view = value;
}

void XM_CALLCONV ReflectionEffect::SetProjection(FXMMATRIX value)
{
	m_pImpl->m_projection = value;
}

void XM_CALLCONV ReflectionEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
	m_pImpl->m_world = world;
	m_pImpl->m_view = view;
	m_pImpl->m_projection = projection;
}

void ReflectionEffect::SetCameraPosition(DirectX::XMFLOAT3 value)
{
	m_pImpl->m_camera = value;
}

void ReflectionEffect::SetTextureEnabled(bool value)
{
	m_pImpl->m_isTextured = value;
}

void ReflectionEffect::SetTexture(_In_opt_ ID3D11ShaderResourceView* value)
{
	m_pImpl->m_texture = value;
}

void ReflectionEffect::SetDiffuseColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->diffuseColor = value;
}

void ReflectionEffect::SetEmissiveColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->emissiveColor = value;
}

void ReflectionEffect::SetAlpha(float value)
{
	m_pImpl->alpha = value;
}

void ReflectionEffect::AddPointLight(XMFLOAT4 Color, XMFLOAT3 Position, float Radius)
{
	if (m_pImpl->NumOfPL < MAX_NUMBER_OF_LIGHT)
	{
		m_pImpl->PointLights[m_pImpl->NumOfPL].Color = Color;
		m_pImpl->PointLights[m_pImpl->NumOfPL].Position = Position;
		m_pImpl->PointLights[m_pImpl->NumOfPL].Radius = Radius;
		m_pImpl->NumOfPL++;
	}
}

void ReflectionEffect::AddDirectLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction)
{
	if (m_pImpl->NumOfDL < MAX_NUMBER_OF_LIGHT)
	{
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].Color = Color;
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].Direction = Direction;
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].EmptyTo16B = m_pImpl->FillTo16;
		m_pImpl->NumOfDL++;
	}
}

void ReflectionEffect::AddSpotLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius)
{
	if (m_pImpl->NumOfSL < MAX_NUMBER_OF_LIGHT)
	{
		m_pImpl->SpotLights[m_pImpl->NumOfSL].Color = Color;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].Direction = Direction;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].Position = Position;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].OuterAngle = OuterAngle;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].InnerAngle = InnerAngle;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].Radius = Radius;
		m_pImpl->SpotLights[m_pImpl->NumOfSL].SpaceTo16B = m_pImpl->Fill3To16;
		m_pImpl->NumOfSL++;
	}
}

void ReflectionEffect::UpdatePointLight(int id, DirectX::XMFLOAT3 Position)
{
	m_pImpl->PointLights[id].Position = Position;
	m_pImpl->isSent = false;
}

void ReflectionEffect::UpdateSpotLight(int id, DirectX::XMFLOAT3 Position)
{
	m_pImpl->SpotLights[id].Position = Position;
	m_pImpl->isSent = false;
}

void ReflectionEffect::SetMaterialName(std::string Name)
{
	m_pImpl->materialName = Name;
}

std::string ReflectionEffect::GetMaterialName()
{
	return m_pImpl->materialName;
}

void ReflectionEffect::SetCubeMap(ID3D11ShaderResourceView * value)
{
	m_pImpl->m_cubeMap = value;
	m_pImpl->m_isCubed = true;
}
