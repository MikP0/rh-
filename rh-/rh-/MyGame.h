#pragma once

#include "pch.h"

class MyGame
{
public:
	MyGame();
	MyGame(ID3D11Device1* dev, ID3D11DeviceContext1* con);
	~MyGame();

	ID3D11Device1* Direct3DDevice() const;
	ID3D11DeviceContext1* Direct3DDeviceContext() const;
	ID3D11Device1* mDirect3DDevice;
	ID3D11DeviceContext1* mDirect3DDeviceContext;
};

