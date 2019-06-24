#include "pch.h"
#include "ShadowEffect.h"

using namespace DirectX;

#define MAX_NUMBER_OF_LIGHT 100

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

	DirectX::XMFLOAT4X4 ShadowMapTransform;

	DirectX::XMFLOAT4 IsNormalMap;
};

struct DynamicConstantShadowBuffer
{
	DirectX::XMFLOAT4X4 WorldViewProjection;
};


class ShadowEffect::Impl
{
public:
	std::string materialName;

	XMMATRIX m_world;
	XMMATRIX m_view;
	XMMATRIX m_projection;
	XMFLOAT3 m_camera;

	XMMATRIX m_shadowMapTransform;

	XMFLOAT3 diffuseColor;
	XMFLOAT3 emissiveColor;
	float alpha;

	XMVECTOR AmbientColor;
	XMVECTOR SpecularColor;
	XMVECTOR SpecularPower;
	XMVECTOR IsTextured;

	XMVECTOR IsNormal;

	bool renderingShadowMap = false;
	bool shadowEnable = false;

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
	bool m_isNormalMapped = false;
	bool m_isShadowMapped = false;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderShadow;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderShadow;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_staticConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicConstantShadowBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMap;

	StaticConstantBuffer m_staticData;
	DynamicConstantBuffer m_dynamicData;
	DynamicConstantShadowBuffer m_dynamicShadowData;

	std::vector<uint8_t> m_VSBytecode;
	std::vector<uint8_t> m_VSBytecodeShadow;


	ShadowEffect::Impl(_In_ ID3D11Device* device);
	void Apply(_In_ ID3D11DeviceContext* deviceContext);
	void GetVertexShaderBytecode(
		_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength);

	HRESULT loadShaders(ID3D11Device* device);

	static void* operator new(size_t size)
	{
		const size_t alignment = _alignof(ShadowEffect::Impl);
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

ShadowEffect::Impl::Impl(_In_ ID3D11Device* device)
{
	m_isInit = false;

	AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularPower = { 0.0 };

	m_camera.x = 0;
	m_camera.y = 0;
	m_camera.z = 0;

	IsTextured = { 0.0f };
	IsNormal = { 0.0f , 0.0f, 0.0f, 0.0f };


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

	CD3D11_BUFFER_DESC dynamicBufferShadowDesc(
		sizeof(DynamicConstantShadowBuffer), D3D11_BIND_CONSTANT_BUFFER);
	if (FAILED(device->CreateBuffer(
		&dynamicBufferShadowDesc, nullptr, &m_dynamicConstantShadowBuffer)))
	{
		return;
	}

	m_isInit = true;
}

void ShadowEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	if (!renderingShadowMap)
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
		XMStoreFloat4x4(&m_dynamicData.ShadowMapTransform, XMMatrixTranspose(XMMatrixMultiply(m_world, m_shadowMapTransform)));


		XMVECTOR cam = { m_camera.x, m_camera.y, m_camera.z };
		XMStoreFloat3(&m_dynamicData.CameraPosition, cam);

		XMStoreFloat(&m_dynamicData.IsTextured, IsTextured);

		if (m_isNormalMapped)
		{		
			IsNormal = { 1.0f , 0.0f, 0.0f, 0.0f };
		}
		else
		{
			IsNormal = { 0.0f , 0.0f, 0.0f, 0.0f };
		}
		XMStoreFloat4(&m_dynamicData.IsNormalMap, IsNormal);



		deviceContext->UpdateSubresource(
			m_dynamicConstantBuffer.Get(), 0, NULL, &m_dynamicData, 0, 0);

		deviceContext->VSSetConstantBuffers(
			1, 1, m_dynamicConstantBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(
			1, 1, m_dynamicConstantBuffer.GetAddressOf());

		ID3D11ShaderResourceView* textures[3];

		if (m_isTextured)
		{
			textures[0] = m_texture.Get();
		}
		else
		{
			textures[0] = nullptr;
		}

		if (m_isShadowMapped)
		{
			textures[1] = m_shadowMap.Get();
		}
		else
		{
			textures[1] = nullptr;
		}

		if (m_isNormalMapped)
		{
			textures[2] = m_normalMap.Get();
		}
		else
		{
			textures[2] = nullptr;
		}


		deviceContext->PSSetShaderResources(
			0, static_cast<UINT>(3), textures);

		deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	}
	else
	{
		if (!m_isInit)
		{
			return;
		}

		XMStoreFloat4x4(&m_dynamicShadowData.WorldViewProjection, XMMatrixTranspose(XMMatrixMultiply((XMMatrixMultiply(m_world, m_view)), m_projection)));

		deviceContext->UpdateSubresource(
			m_dynamicConstantShadowBuffer.Get(), 0, NULL, &m_dynamicShadowData, 0, 0);

		deviceContext->VSSetConstantBuffers(
			1, 1, m_dynamicConstantShadowBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(
			1, 1, m_dynamicConstantShadowBuffer.GetAddressOf());

	
		deviceContext->VSSetShader(m_vertexShaderShadow.Get(), nullptr, 0);
		deviceContext->PSSetShader(m_pixelShaderShadow.Get(), nullptr, 0);
	}
}

void ShadowEffect::Impl::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	if (!renderingShadowMap)
	{
		*pShaderByteCode = m_VSBytecode.empty() ? nullptr : &m_VSBytecode[0];
		*pByteCodeLength = m_VSBytecode.size();
	}
	else
	{
		*pShaderByteCode = m_VSBytecodeShadow.empty() ? nullptr : &m_VSBytecodeShadow[0];
		*pByteCodeLength = m_VSBytecodeShadow.size();
	}
}

HRESULT ShadowEffect::Impl::loadShaders(ID3D11Device* device)
{
	try
	{
		m_VSBytecode = DX::ReadData(L"VSShadowShader.cso");
		DX::ThrowIfFailed(device->CreateVertexShader(
			m_VSBytecode.data(),
			m_VSBytecode.size(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()));

		auto psData = DX::ReadData(L"PSShadowShader.cso");
		DX::ThrowIfFailed(device->CreatePixelShader(
			psData.data(),
			psData.size(),
			nullptr,
			m_pixelShader.ReleaseAndGetAddressOf()));

		m_VSBytecodeShadow = DX::ReadData(L"VSShadowMap.cso");
		DX::ThrowIfFailed(device->CreateVertexShader(
			m_VSBytecodeShadow.data(),
			m_VSBytecodeShadow.size(),
			nullptr,
			m_vertexShaderShadow.ReleaseAndGetAddressOf()));

		auto psDataShadow = DX::ReadData(L"PSShadowMap.cso");
		DX::ThrowIfFailed(device->CreatePixelShader(
			psDataShadow.data(),
			psDataShadow.size(),
			nullptr,
			m_pixelShaderShadow.ReleaseAndGetAddressOf()));
	}
	catch (...)
	{
		return E_FAIL;
	}
	return S_OK;
}

ShadowEffect::ShadowEffect(_In_ ID3D11Device* device)
	: m_pImpl(new Impl(device))
{
}

ShadowEffect::~ShadowEffect() {}

ShadowEffect::ShadowEffect(ShadowEffect&& moveFrom)
	: m_pImpl(std::move(moveFrom.m_pImpl))
{
}

ShadowEffect& ShadowEffect::operator=(ShadowEffect&& moveFrom)
{
	m_pImpl = std::move(moveFrom.m_pImpl);
	return *this;
}

void ShadowEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	m_pImpl->Apply(deviceContext);
}

void ShadowEffect::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	m_pImpl->GetVertexShaderBytecode(pShaderByteCode, pByteCodeLength);
}

void XM_CALLCONV ShadowEffect::SetWorld(FXMMATRIX value)
{
	m_pImpl->m_world = value;
}

void XM_CALLCONV ShadowEffect::SetView(FXMMATRIX value)
{
	m_pImpl->m_view = value;
}

void XM_CALLCONV ShadowEffect::SetProjection(FXMMATRIX value)
{
	m_pImpl->m_projection = value;
}

void XM_CALLCONV ShadowEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
	m_pImpl->m_world = world;
	m_pImpl->m_view = view;
	m_pImpl->m_projection = projection;
}

void ShadowEffect::SetCameraPosition(DirectX::XMFLOAT3 value)
{
	m_pImpl->m_camera = value;
}

void ShadowEffect::SetTextureEnabled(bool value)
{
	m_pImpl->m_isTextured = value;
}

void ShadowEffect::SetTexture(_In_opt_ ID3D11ShaderResourceView* value)
{
	m_pImpl->m_texture = value;
}

void ShadowEffect::SetNormalMapEnabled(bool value)
{
	m_pImpl->m_isNormalMapped = value;
}

void ShadowEffect::SetNormalMap(_In_opt_ ID3D11ShaderResourceView* value)
{
	m_pImpl->m_normalMap = value;
}

void ShadowEffect::SetDiffuseColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->diffuseColor = value;
}

void ShadowEffect::SetEmissiveColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->emissiveColor = value;
}

void ShadowEffect::SetAlpha(float value)
{
	m_pImpl->alpha = value;
}

void ShadowEffect::AddPointLight(XMFLOAT4 Color, XMFLOAT3 Position, float Radius)
{
	if (m_pImpl->NumOfPL < MAX_NUMBER_OF_LIGHT)
	{
		m_pImpl->PointLights[m_pImpl->NumOfPL].Color = Color;
		m_pImpl->PointLights[m_pImpl->NumOfPL].Position = Position;
		m_pImpl->PointLights[m_pImpl->NumOfPL].Radius = Radius;
		m_pImpl->NumOfPL++;
	}
}

void ShadowEffect::AddDirectLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction)
{
	if (m_pImpl->NumOfDL < MAX_NUMBER_OF_LIGHT)
	{
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].Color = Color;
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].Direction = Direction;
		m_pImpl->DirectionalLights[m_pImpl->NumOfDL].EmptyTo16B = m_pImpl->FillTo16;
		m_pImpl->NumOfDL++;
	}
}

void ShadowEffect::AddSpotLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius)
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

void ShadowEffect::UpdatePointLight(int id, DirectX::XMFLOAT3 Position)
{
	m_pImpl->PointLights[id].Position = Position;
	m_pImpl->isSent = false;
}

void ShadowEffect::UpdateSpotLight(int id, DirectX::XMFLOAT3 Position)
{
	m_pImpl->SpotLights[id].Position = Position;
	m_pImpl->isSent = false;
}

void ShadowEffect::SetRenderingShadowMap(bool value)
{
	m_pImpl->renderingShadowMap = value;
}

void ShadowEffect::SetShadowEnable(bool value)
{
	m_pImpl->shadowEnable = value;
}

void ShadowEffect::SetShadowMapTransform(FXMMATRIX value)
{
	m_pImpl->m_shadowMapTransform = value;
}

void ShadowEffect::SetShadowMapEnabled(bool value)
{
	m_pImpl->m_isShadowMapped = value;
}

void ShadowEffect::SetShadowMap(ID3D11ShaderResourceView * value)
{
	m_pImpl->m_shadowMap = value;
}

void ShadowEffect::SetMaterialName(std::string Name)
{
	m_pImpl->materialName = Name;
}

std::string ShadowEffect::GetMaterialName()
{
	return m_pImpl->materialName;
}

