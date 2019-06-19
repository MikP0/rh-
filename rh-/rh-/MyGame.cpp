#include "pch.h"
#include "MyGame.h"


MyGame::MyGame()
{
}

MyGame::MyGame(ID3D11Device1 * dev, ID3D11DeviceContext1* con)
{
	mDirect3DDevice = dev;
	mDirect3DDeviceContext = con;
}

MyGame::~MyGame()
{
}

ID3D11Device1* MyGame::Direct3DDevice() const
{
	return mDirect3DDevice;
}

ID3D11DeviceContext1* MyGame::Direct3DDeviceContext() const
{
	return mDirect3DDeviceContext;
}
