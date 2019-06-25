#include "pch.h"
#include "ReflectionFactory.h"
#include "ReflectionEffect.h"
#include <map>
#include <mutex>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#pragma warning(disable : 4351)

class ReflectionFactory::Impl
{
public:
	Impl(_In_ ID3D11Device* device)
		: mPath{}
		, m_device(device)
		, mSharing(true)
		, mUseNormalMapEffect(true)
		, mForceSRGB(false)
	{}

	std::shared_ptr<IEffect> CreateEffect(_In_ IEffectFactory* factory, _In_ const IEffectFactory::EffectInfo& info, _In_opt_ ID3D11DeviceContext* deviceContext);
	void CreateTexture(_In_z_ const wchar_t* texture, _In_opt_ ID3D11DeviceContext* deviceContext, _Outptr_ ID3D11ShaderResourceView** textureView);
	void ReleaseCache();

	wchar_t mPath[MAX_PATH];

	bool mSharing;
	bool mUseNormalMapEffect;
	bool mForceSRGB;

	typedef std::map< std::wstring, std::shared_ptr<ReflectionEffect> > EffectCache;
	EffectCache  mEffectCache;

private:

	typedef std::map< std::wstring, ComPtr<ID3D11ShaderResourceView> > TextureCache;

	TextureCache mTextureCache;

	ComPtr<ID3D11Device> m_device;
	std::mutex mutex;
};

_Use_decl_annotations_ std::shared_ptr<IEffect> ReflectionFactory::Impl::CreateEffect(IEffectFactory* factory, const IEffectFactory::EffectInfo& info, ID3D11DeviceContext* deviceContext)
{
	if (mSharing && info.name && *info.name)
	{
		auto it = mEffectCache.find(info.name);
		if (mSharing && it != mEffectCache.end())
		{
			return it->second;
		}
	}

	auto effect = std::make_shared<ReflectionEffect>(m_device.Get());

	if (info.diffuseTexture && *info.diffuseTexture)
	{
		ComPtr<ID3D11ShaderResourceView> srv;

		factory->CreateTexture(info.diffuseTexture, deviceContext, srv.GetAddressOf());

		effect->SetTexture(srv.Get());
		effect->SetTextureEnabled(true);
	}
	else
	{
		effect->SetDiffuseColor(info.diffuseColor);
		effect->SetEmissiveColor(info.emissiveColor);
		effect->SetAlpha(info.alpha);
	}

	if (info.normalTexture && *info.normalTexture)
	{
		ComPtr<ID3D11ShaderResourceView> srv;
		factory->CreateTexture(info.normalTexture, deviceContext, srv.GetAddressOf());
		effect->SetTexture(srv.Get());
	}

	if (mSharing && info.name && *info.name)
	{
		const wchar_t* txt = info.name;
		std::wstring ws(txt);
		std::string std(ws.begin(), ws.end());
		effect->SetMaterialName(std);

		std::lock_guard<std::mutex> lock(mutex);
		EffectCache::value_type v(info.name, effect);
		mEffectCache.insert(v);
	}

	return effect;
}

_Use_decl_annotations_ void ReflectionFactory::Impl::CreateTexture(const wchar_t* name, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureView)
{
	if (!name || !textureView)
		throw std::exception("invalid arguments");

#if defined(_XBOX_ONE) && defined(_TITLE)
	UNREFERENCED_PARAMETER(deviceContext);
#endif

	auto it = mTextureCache.find(name);

	if (mSharing && it != mTextureCache.end())
	{
		ID3D11ShaderResourceView* srv = it->second.Get();
		srv->AddRef();
		*textureView = srv;
	}
	else
	{
		wchar_t fullName[MAX_PATH] = {};
		wcscpy_s(fullName, mPath);
		wcscat_s(fullName, name);

		WIN32_FILE_ATTRIBUTE_DATA fileAttr = {};
		if (!GetFileAttributesExW(fullName, GetFileExInfoStandard, &fileAttr))
		{
			// Try Current Working Directory (CWD)
			wcscpy_s(fullName, name);
			if (!GetFileAttributesExW(fullName, GetFileExInfoStandard, &fileAttr))
			{
				throw std::exception("CreateTexture");
			}
		}

		wchar_t ext[_MAX_EXT];
		_wsplitpath_s(name, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		if (_wcsicmp(ext, L".dds") == 0)
		{
			HRESULT hr = CreateDDSTextureFromFileEx(
				m_device.Get(), fullName, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				mForceSRGB, nullptr, textureView);
			if (FAILED(hr))
			{
				throw std::exception("CreateDDSTextureFromFile");
			}
		}
#if !defined(_XBOX_ONE) || !defined(_TITLE)
		else if (deviceContext)
		{
			std::lock_guard<std::mutex> lock(mutex);
			HRESULT hr = CreateWICTextureFromFileEx(
				m_device.Get(), deviceContext, fullName, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				mForceSRGB ? WIC_LOADER_FORCE_SRGB : WIC_LOADER_DEFAULT, nullptr, textureView);
			if (FAILED(hr))
			{
				throw std::exception("CreateWICTextureFromFile");
			}
		}
#endif
		else
		{
			HRESULT hr = CreateWICTextureFromFileEx(
				m_device.Get(), fullName, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				mForceSRGB ? WIC_LOADER_FORCE_SRGB : WIC_LOADER_DEFAULT, nullptr, textureView);
			if (FAILED(hr))
			{
				throw std::exception("CreateWICTextureFromFile");
			}
		}

		if (mSharing && *name && it == mTextureCache.end())
		{
			std::lock_guard<std::mutex> lock(mutex);
			TextureCache::value_type v(name, *textureView);
			mTextureCache.insert(v);
		}
	}
}

void ReflectionFactory::Impl::ReleaseCache()
{
	std::lock_guard<std::mutex> lock(mutex);
	mEffectCache.clear();
	mTextureCache.clear();
}

ReflectionFactory::ReflectionFactory(_In_ ID3D11Device* device) : pImpl(std::make_unique<ReflectionFactory::Impl>(device))
{
}

ReflectionFactory::~ReflectionFactory()
{
}

ReflectionFactory::ReflectionFactory(ReflectionFactory&& moveFrom) : pImpl(std::move(moveFrom.pImpl))
{
}

ReflectionFactory& ReflectionFactory::operator=(ReflectionFactory&& moveFrom)
{
	pImpl = std::move(moveFrom.pImpl);
	return *this;
}

_Use_decl_annotations_ std::shared_ptr<IEffect> ReflectionFactory::CreateEffect(const EffectInfo& info, ID3D11DeviceContext* deviceContext)
{
	return pImpl->CreateEffect(this, info, deviceContext);
}

_Use_decl_annotations_ void ReflectionFactory::CreateTexture(const wchar_t* name, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureView)
{
	return pImpl->CreateTexture(name, deviceContext, textureView);
}

void ReflectionFactory::ReleaseCache()
{
	pImpl->ReleaseCache();
}

void ReflectionFactory::SetSharing(bool enabled)
{
	UNREFERENCED_PARAMETER(enabled);
}

void ReflectionFactory::EnableNormalMapEffect(bool enabled)
{
	UNREFERENCED_PARAMETER(enabled);
}

void ReflectionFactory::EnableForceSRGB(bool forceSRGB)
{
	UNREFERENCED_PARAMETER(forceSRGB);
}

void ReflectionFactory::SetDirectory(_In_opt_z_ const wchar_t* path)
{
	if (path && *path != 0)
	{
		wcscpy_s(pImpl->mPath, path);
		size_t len = wcsnlen(pImpl->mPath, MAX_PATH);
		if (len > 0 && len < (MAX_PATH - 1))
		{
			// Ensure it has a trailing slash
			if (pImpl->mPath[len - 1] != L'\\')
			{
				pImpl->mPath[len] = L'\\';
				pImpl->mPath[len + 1] = 0;
			}
		}
	}
	else
		*pImpl->mPath = 0;
}

void ReflectionFactory::SetCameraPosition(DirectX::XMFLOAT3 value)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->SetCameraPosition(value);
	}
}

void ReflectionFactory::SetCubeMap(ID3D11ShaderResourceView * value)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->SetCubeMap(value);
	}
}

void ReflectionFactory::AddPointLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Position, float Radius)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->AddPointLight(Color, Position, Radius);
	}
}

void ReflectionFactory::AddDirectLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->AddDirectLight(Color, Direction);
	}
}

void ReflectionFactory::AddSpotLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->AddSpotLight(Color, Direction, OuterAngle, Position, InnerAngle, Radius);
	}
}

void ReflectionFactory::UpdatePointLight(int id, DirectX::XMFLOAT3 Position)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->UpdatePointLight(id, Position);
	}
}

void ReflectionFactory::UpdateSpotLight(int id, DirectX::XMFLOAT3 Position)
{
	for (std::map< std::wstring, std::shared_ptr<ReflectionEffect> >::const_iterator it = pImpl->mEffectCache.begin(); it != pImpl->mEffectCache.end(); ++it)
	{
		it->second->UpdateSpotLight(id, Position);
	}
}

