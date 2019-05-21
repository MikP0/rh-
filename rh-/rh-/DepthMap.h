#pragma once
#include "pch.h"
#include "RenderTarget.h"

class MyGame;

class DepthMap : public RenderTarget
{
	RTTI_DECLARATIONS(DepthMap, RenderTarget)

public:
	DepthMap(MyGame& game, UINT width, UINT height);
	~DepthMap();

	ID3D11ShaderResourceView* OutputTexture() const;
	ID3D11DepthStencilView* DepthStencilView() const;

	virtual void Begin() override;
	virtual void End() override;

private:
	DepthMap();
	DepthMap(const DepthMap& rhs);
	DepthMap& operator=(const DepthMap& rhs);

	MyGame* mGame;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11ShaderResourceView* mOutputTexture;
	D3D11_VIEWPORT mViewport;
};