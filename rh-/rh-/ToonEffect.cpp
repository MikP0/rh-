#include "ToonEffect.h"

using namespace DirectX;

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

	POINT_LIGHT PointLight[3];
	DIRECTIONAL_LIGHT DirectionalLight[3];
	SPOT_LIGHT SpotLight[3];
};

struct DynamicConstantBuffer
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 WorldViewProjection;
	DirectX::XMFLOAT3 CameraPosition;
	float IsTextured;
};




class ToonEffect::Impl
{
public:

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



	XMVECTOR FillTo16 = { 0.0f };
	XMVECTOR Fill3To16 = { 0.0f, 0.0f, 0.0f };

	bool isSent = false;

	bool m_isInit = false;
	bool m_isTextured = false;
	bool m_isNormalMapped = false;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_staticConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalMap;
	StaticConstantBuffer m_staticData;
	DynamicConstantBuffer m_dynamicData;
	std::vector<uint8_t> m_VSBytecode;


	ToonEffect::Impl(_In_ ID3D11Device* device);
	void Apply(_In_ ID3D11DeviceContext* deviceContext);
	void GetVertexShaderBytecode(
		_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength);

	HRESULT loadShaders(ID3D11Device* device);

	static void* operator new(size_t size)
	{
		const size_t alignment = _alignof(ToonEffect::Impl);
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

ToonEffect::Impl::Impl(_In_ ID3D11Device* device)
{
	m_isInit = false;

	AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	SpecularPower = { 0.0 };

	static const DirectX::XMVECTOR temp = { 0.0f };
	static const DirectX::XMVECTOR temptemp = { 0.0f, 0.0f, 0.0f };
	static const DirectX::XMVECTOR temptemptemp = { 0.0f, 0.0f, 0.0f, 0.0f };


	XMStoreFloat4(&m_staticData.DiffuseColor, temptemptemp);
	XMStoreFloat3(&m_staticData.EmissiveColor, temptemp);
	XMStoreFloat(&m_staticData.SpecularPower, SpecularPower);
	XMStoreFloat4(&m_staticData.AmbientColor, AmbientColor);
	XMStoreFloat4(&m_staticData.SpecularColor, SpecularColor);


	static const DirectX::XMVECTOR lightData1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightData1Pos = { -2.0f, 0.0f, -2.0f };
	static const DirectX::XMVECTOR lightData1Ros = { 3.0f };
	static const DirectX::XMVECTOR lightData2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightData2Pos = { -2.0f, 0.0f, 2.0f };
	static const DirectX::XMVECTOR lightData2Ros = { 3.0f };
	static const DirectX::XMVECTOR lightData3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightData3Pos = { 2.0f, -1.0f, 1.0f };
	static const DirectX::XMVECTOR lightData3Ros = { 3.0f };
	XMStoreFloat4(&m_staticData.PointLight[0].Color, lightData1Col);
	XMStoreFloat3(&m_staticData.PointLight[0].Position, lightData1Pos);
	XMStoreFloat(&m_staticData.PointLight[0].Radius, lightData1Ros);
	XMStoreFloat4(&m_staticData.PointLight[1].Color, lightData2Col);
	XMStoreFloat3(&m_staticData.PointLight[1].Position, lightData2Pos);
	XMStoreFloat(&m_staticData.PointLight[1].Radius, lightData2Ros);
	XMStoreFloat4(&m_staticData.PointLight[2].Color, lightData3Col);
	XMStoreFloat3(&m_staticData.PointLight[2].Position, lightData3Pos);
	XMStoreFloat(&m_staticData.PointLight[2].Radius, lightData3Ros);


	static const DirectX::XMVECTOR lightSun1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightSun1Dir = { 0.0f, -1.0f, -3.0f };
	static const DirectX::XMVECTOR lightSun2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightSun2Dir = { -2.0f, 0.0f, -2.0f };
	static const DirectX::XMVECTOR lightSun3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightSun3Dir = { 2.0f, 0.0f, 2.0f };
	XMStoreFloat4(&m_staticData.DirectionalLight[0].Color, lightSun1Col);
	XMStoreFloat3(&m_staticData.DirectionalLight[0].Direction, lightSun1Dir);
	XMStoreFloat4(&m_staticData.DirectionalLight[1].Color, lightSun2Col);
	XMStoreFloat3(&m_staticData.DirectionalLight[1].Direction, lightSun2Dir);
	XMStoreFloat4(&m_staticData.DirectionalLight[2].Color, lightSun3Col);
	XMStoreFloat3(&m_staticData.DirectionalLight[2].Direction, lightSun3Dir);
	XMStoreFloat(&m_staticData.DirectionalLight[0].EmptyTo16B, FillTo16);
	XMStoreFloat(&m_staticData.DirectionalLight[1].EmptyTo16B, FillTo16);
	XMStoreFloat(&m_staticData.DirectionalLight[2].EmptyTo16B, FillTo16);


	static const DirectX::XMVECTOR lightRef1Dir = { 0.0f, 0.0f, 1.0f };
	static const DirectX::XMVECTOR lightRef1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightRef1Pos = { 0.0f, 3.0f, 0.0f };
	static const DirectX::XMVECTOR lightRef1OuterAngle = { 0.25f };
	static const DirectX::XMVECTOR lightRef1InnerAngle = { 0.75f };
	static const DirectX::XMVECTOR lightRef1LightRadius = { 10.0f };
	static const DirectX::XMVECTOR lightRef2Dir = { 0.0f, 0.0f, -2.0f };
	static const DirectX::XMVECTOR lightRef2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightRef2Pos = { 0.0f, 2.0f, 0.0f };
	static const DirectX::XMVECTOR lightRef2OuterAngle = { 0.25f };
	static const DirectX::XMVECTOR lightRef2InnerAngle = { 0.75f };
	static const DirectX::XMVECTOR lightRef2LightRadius = { 10.0f };
	static const DirectX::XMVECTOR lightRef3Dir = { 0.0f, 0.0f, -2.0f };
	static const DirectX::XMVECTOR lightRef3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const DirectX::XMVECTOR lightRef3Pos = { 0.0f, 2.0f, 0.0f };
	static const DirectX::XMVECTOR lightRef3OuterAngle = { 0.25f };
	static const DirectX::XMVECTOR lightRef3InnerAngle = { 0.75f };
	static const DirectX::XMVECTOR lightRef3LightRadius = { 10.0f };


	XMStoreFloat3(&m_staticData.SpotLight[0].Direction, lightRef1Dir);
	XMStoreFloat4(&m_staticData.SpotLight[0].Color, lightRef1Col);
	XMStoreFloat3(&m_staticData.SpotLight[0].Position, lightRef1Pos);
	XMStoreFloat(&m_staticData.SpotLight[0].OuterAngle, lightRef1OuterAngle);
	XMStoreFloat(&m_staticData.SpotLight[0].InnerAngle, lightRef1InnerAngle);
	XMStoreFloat(&m_staticData.SpotLight[0].Radius, lightRef1LightRadius);

	XMStoreFloat3(&m_staticData.SpotLight[1].Direction, lightRef2Dir);
	XMStoreFloat4(&m_staticData.SpotLight[1].Color, lightRef2Col);
	XMStoreFloat3(&m_staticData.SpotLight[1].Position, lightRef2Pos);
	XMStoreFloat(&m_staticData.SpotLight[1].OuterAngle, lightRef2OuterAngle);
	XMStoreFloat(&m_staticData.SpotLight[1].InnerAngle, lightRef2InnerAngle);
	XMStoreFloat(&m_staticData.SpotLight[1].Radius, lightRef2LightRadius);

	XMStoreFloat3(&m_staticData.SpotLight[2].Direction, lightRef3Dir);
	XMStoreFloat4(&m_staticData.SpotLight[2].Color, lightData3Col);
	XMStoreFloat3(&m_staticData.SpotLight[2].Position, lightData3Pos);
	XMStoreFloat(&m_staticData.SpotLight[2].OuterAngle, lightRef3OuterAngle);
	XMStoreFloat(&m_staticData.SpotLight[2].InnerAngle, lightRef3InnerAngle);
	XMStoreFloat(&m_staticData.SpotLight[2].Radius, lightRef3LightRadius);

	XMStoreFloat3(&m_staticData.SpotLight[0].SpaceTo16B, Fill3To16);
	XMStoreFloat3(&m_staticData.SpotLight[1].SpaceTo16B, Fill3To16);
	XMStoreFloat3(&m_staticData.SpotLight[2].SpaceTo16B, Fill3To16);


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

void ToonEffect::Impl::Apply(_In_ ID3D11DeviceContext* deviceContext)
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




			static const DirectX::XMVECTOR lightData1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightData1Pos = { -2.0f, 0.0f, -2.0f };
			static const DirectX::XMVECTOR lightData1Ros = { 3.0f };
			static const DirectX::XMVECTOR lightData2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightData2Pos = { -2.0f, 0.0f, 2.0f };
			static const DirectX::XMVECTOR lightData2Ros = { 3.0f };
			static const DirectX::XMVECTOR lightData3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightData3Pos = { 2.0f, -1.0f, 1.0f };
			static const DirectX::XMVECTOR lightData3Ros = { 3.0f };
			XMStoreFloat4(&m_staticData.PointLight[0].Color, lightData1Col);
			XMStoreFloat3(&m_staticData.PointLight[0].Position, lightData1Pos);
			XMStoreFloat(&m_staticData.PointLight[0].Radius, lightData1Ros);
			XMStoreFloat4(&m_staticData.PointLight[1].Color, lightData2Col);
			XMStoreFloat3(&m_staticData.PointLight[1].Position, lightData2Pos);
			XMStoreFloat(&m_staticData.PointLight[1].Radius, lightData2Ros);
			XMStoreFloat4(&m_staticData.PointLight[2].Color, lightData3Col);
			XMStoreFloat3(&m_staticData.PointLight[2].Position, lightData3Pos);
			XMStoreFloat(&m_staticData.PointLight[2].Radius, lightData3Ros);

			static const DirectX::XMVECTOR lightSun1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightSun1Dir = { 0.0f, -1.0f, -3.0f };
			static const DirectX::XMVECTOR lightSun2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightSun2Dir = { -2.0f, 0.0f, -2.0f };
			static const DirectX::XMVECTOR lightSun3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightSun3Dir = { 2.0f, 0.0f, 2.0f };
			XMStoreFloat4(&m_staticData.DirectionalLight[0].Color, lightSun1Col);
			XMStoreFloat3(&m_staticData.DirectionalLight[0].Direction, lightSun1Dir);
			XMStoreFloat4(&m_staticData.DirectionalLight[1].Color, lightSun2Col);
			XMStoreFloat3(&m_staticData.DirectionalLight[1].Direction, lightSun2Dir);
			XMStoreFloat4(&m_staticData.DirectionalLight[2].Color, lightSun3Col);
			XMStoreFloat3(&m_staticData.DirectionalLight[2].Direction, lightSun3Dir);
			XMStoreFloat(&m_staticData.DirectionalLight[0].EmptyTo16B, FillTo16);
			XMStoreFloat(&m_staticData.DirectionalLight[1].EmptyTo16B, FillTo16);
			XMStoreFloat(&m_staticData.DirectionalLight[2].EmptyTo16B, FillTo16);


			static const DirectX::XMVECTOR lightRef1Dir = { 0.0f, 0.0f, 1.0f };
			static const DirectX::XMVECTOR lightRef1Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightRef1Pos = { 0.0f, 3.0f, 0.0f };
			static const DirectX::XMVECTOR lightRef1OuterAngle = { 0.25f };
			static const DirectX::XMVECTOR lightRef1InnerAngle = { 0.75f };
			static const DirectX::XMVECTOR lightRef1LightRadius = { 10.0f };
			static const DirectX::XMVECTOR lightRef2Dir = { 0.0f, 0.0f, -2.0f };
			static const DirectX::XMVECTOR lightRef2Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightRef2Pos = { 0.0f, 2.0f, 0.0f };
			static const DirectX::XMVECTOR lightRef2OuterAngle = { 0.25f };
			static const DirectX::XMVECTOR lightRef2InnerAngle = { 0.75f };
			static const DirectX::XMVECTOR lightRef2LightRadius = { 10.0f };
			static const DirectX::XMVECTOR lightRef3Dir = { 0.0f, 0.0f, -2.0f };
			static const DirectX::XMVECTOR lightRef3Col = { 1.0f, 1.0f, 1.0f, 1.0f };
			static const DirectX::XMVECTOR lightRef3Pos = { 0.0f, 2.0f, 0.0f };
			static const DirectX::XMVECTOR lightRef3OuterAngle = { 0.25f };
			static const DirectX::XMVECTOR lightRef3InnerAngle = { 0.75f };
			static const DirectX::XMVECTOR lightRef3LightRadius = { 10.0f };


			XMStoreFloat3(&m_staticData.SpotLight[0].Direction, lightRef1Dir);
			XMStoreFloat4(&m_staticData.SpotLight[0].Color, lightRef1Col);
			XMStoreFloat3(&m_staticData.SpotLight[0].Position, lightRef1Pos);
			XMStoreFloat(&m_staticData.SpotLight[0].OuterAngle, lightRef1OuterAngle);
			XMStoreFloat(&m_staticData.SpotLight[0].InnerAngle, lightRef1InnerAngle);
			XMStoreFloat(&m_staticData.SpotLight[0].Radius, lightRef1LightRadius);

			XMStoreFloat3(&m_staticData.SpotLight[1].Direction, lightRef2Dir);
			XMStoreFloat4(&m_staticData.SpotLight[1].Color, lightRef2Col);
			XMStoreFloat3(&m_staticData.SpotLight[1].Position, lightRef2Pos);
			XMStoreFloat(&m_staticData.SpotLight[1].OuterAngle, lightRef2OuterAngle);
			XMStoreFloat(&m_staticData.SpotLight[1].InnerAngle, lightRef2InnerAngle);
			XMStoreFloat(&m_staticData.SpotLight[1].Radius, lightRef2LightRadius);

			XMStoreFloat3(&m_staticData.SpotLight[2].Direction, lightRef3Dir);
			XMStoreFloat4(&m_staticData.SpotLight[2].Color, lightData3Col);
			XMStoreFloat3(&m_staticData.SpotLight[2].Position, lightData3Pos);
			XMStoreFloat(&m_staticData.SpotLight[2].OuterAngle, lightRef3OuterAngle);
			XMStoreFloat(&m_staticData.SpotLight[2].InnerAngle, lightRef3InnerAngle);
			XMStoreFloat(&m_staticData.SpotLight[2].Radius, lightRef3LightRadius);

			XMStoreFloat3(&m_staticData.SpotLight[0].SpaceTo16B, Fill3To16);
			XMStoreFloat3(&m_staticData.SpotLight[1].SpaceTo16B, Fill3To16);
			XMStoreFloat3(&m_staticData.SpotLight[2].SpaceTo16B, Fill3To16);
		}
		else
		{
			IsTextured = { 1.0f };
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

	XMVECTOR cam = { m_camera.x, m_camera.y, m_camera.z };
	XMStoreFloat3(&m_dynamicData.CameraPosition, cam);

	XMStoreFloat(&m_dynamicData.IsTextured, IsTextured);


	deviceContext->UpdateSubresource(
		m_dynamicConstantBuffer.Get(), 0, NULL, &m_dynamicData, 0, 0);

	deviceContext->VSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(
		1, 1, m_dynamicConstantBuffer.GetAddressOf());

	ID3D11ShaderResourceView* textures[2];

	if (m_isTextured)
	{
		textures[0] = m_texture.Get();
	}
	else
	{
		textures[0] = nullptr;
	}

	if (m_isNormalMapped)
	{
		textures[1] = m_normalMap.Get();
	}
	else
	{
		textures[1] = nullptr;
	}

	deviceContext->PSSetShaderResources(
		0, static_cast<UINT>(2), textures);

	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void ToonEffect::Impl::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	*pShaderByteCode = m_VSBytecode.empty() ? nullptr : &m_VSBytecode[0];
	*pByteCodeLength = m_VSBytecode.size();
}

HRESULT ToonEffect::Impl::loadShaders(ID3D11Device* device)
{
	try
	{
		m_VSBytecode = DX::ReadData(L"VertexShader.cso");
		DX::ThrowIfFailed(device->CreateVertexShader(
			m_VSBytecode.data(),
			m_VSBytecode.size(),
			nullptr,
			m_vertexShader.ReleaseAndGetAddressOf()));

		auto psData = DX::ReadData(L"PixelShader.cso");
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

ToonEffect::ToonEffect(_In_ ID3D11Device* device)
	: m_pImpl(new Impl(device))
{
}

ToonEffect::~ToonEffect() {}

ToonEffect::ToonEffect(ToonEffect&& moveFrom)
	: m_pImpl(std::move(moveFrom.m_pImpl))
{
}

ToonEffect& ToonEffect::operator=(ToonEffect&& moveFrom)
{
	m_pImpl = std::move(moveFrom.m_pImpl);
	return *this;
}

void ToonEffect::Apply(_In_ ID3D11DeviceContext* deviceContext)
{
	m_pImpl->Apply(deviceContext);
}

void ToonEffect::GetVertexShaderBytecode(
	_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength)
{
	m_pImpl->GetVertexShaderBytecode(pShaderByteCode, pByteCodeLength);
}

void XM_CALLCONV ToonEffect::SetWorld(FXMMATRIX value)
{
	m_pImpl->m_world = value;
}

void XM_CALLCONV ToonEffect::SetView(FXMMATRIX value)
{
	m_pImpl->m_view = value;
}

void XM_CALLCONV ToonEffect::SetProjection(FXMMATRIX value)
{
	m_pImpl->m_projection = value;
}

void XM_CALLCONV ToonEffect::SetMatrices(FXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
	m_pImpl->m_world = world;
	m_pImpl->m_view = view;
	m_pImpl->m_projection = projection;
}

void ToonEffect::SetCameraPosition(DirectX::XMFLOAT3 value)
{
	m_pImpl->m_camera = value;
}

void ToonEffect::SetTextureEnabled(bool value)
{
	m_pImpl->m_isTextured = value;
}

void ToonEffect::SetTexture(_In_opt_ ID3D11ShaderResourceView* value)
{
	m_pImpl->m_texture = value;
}

void ToonEffect::SetNormalMapEnabled(bool value)
{
	m_pImpl->m_isNormalMapped = value;
}

void ToonEffect::SetNormalMap(_In_opt_ ID3D11ShaderResourceView* value)
{
	m_pImpl->m_normalMap = value;
}

void ToonEffect::SetDiffuseColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->diffuseColor = value;
}

void ToonEffect::SetEmissiveColor(DirectX::XMFLOAT3 value)
{
	m_pImpl->emissiveColor = value;
}

void ToonEffect::SetAlpha(float value)
{
	m_pImpl->alpha = value;
}
