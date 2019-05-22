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

	float EuklideanDistance(dxmath::Vector3, dxmath::Vector3);
	float ManhattanDistance(dxmath::Vector3, dxmath::Vector3);
	float DiagonalDistance(dxmath::Vector3, dxmath::Vector3);
	float HexDistance(dxmath::Vector3, dxmath::Vector3);

	void Draw(Camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>);
	std::vector<std::shared_ptr<MapTile>> GetPath(std::shared_ptr<MapTile>, std::shared_ptr<MapTile>);

	bool CanWalk(dxmath::Vector3);
	bool Within(std::shared_ptr<MapTile>);
	std::shared_ptr<MapTile> GetTileWithPosition(dxmath::Vector3);

	ID3D11DeviceContext1* context;
	std::vector<std::shared_ptr<MapTile>> tiles;
	int widthInTiles;
	int heightInTiles;
	const int tileSize = 1.f;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;
};

