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
	void CalculateWalkability();
	void ConnectNeighboringTiles();
	void CreateTileSets();

	void Draw(Camera, ID3D11Device1*);

	std::shared_ptr<MapTile> GetTile(int, int);
	bool Within(dxmath::Vector2);

	std::vector<std::shared_ptr<MapTile>> tiles;
	int widthInTiles;
	int heightInTiles;
	const int tileSize = 2;
};

