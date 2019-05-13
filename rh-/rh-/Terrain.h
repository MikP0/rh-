#pragma once
#include "pch.h"
#include "MapTile.h"
#include "Camera.h"

class Terrain
{
public:
	Terrain();
	virtual ~Terrain();

	void InitTileMap(ID3D11DeviceContext1*);
	void ResetTileMap();
	void SetTilePositionsAndTypes();
	void ConnectNeighboringTiles();

	void Draw(Camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>);

	bool CanWalk(dxmath::Vector3);
	std::shared_ptr<MapTile> GetTileWithPosition(dxmath::Vector3);

	ID3D11DeviceContext1* context;
	std::vector<std::shared_ptr<MapTile>> tiles;
	int widthInTiles;
	int heightInTiles;
	const int tileSize = 1.f;
};

