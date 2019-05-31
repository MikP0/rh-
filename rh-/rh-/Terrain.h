#pragma once
#include "pch.h"
#include "MapTile.h"
#include "Camera.h"
#include "ColliderTypes.h"

using namespace std;

typedef shared_ptr<MapTile> MapTilePtr;
typedef std::shared_ptr<ColliderBase> ColliderBasePtr;

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
	void MakeOcupied(dxmath::Vector3);
	void Update(vector<ColliderBasePtr>);
	void ClearTiles();
	void CoverTilesInRange(dxmath::Vector3, float);

	float EuklideanDistance(dxmath::Vector3, dxmath::Vector3);
	float ManhattanDistance(dxmath::Vector3, dxmath::Vector3);
	float DiagonalDistance(dxmath::Vector3, dxmath::Vector3);
	float HexDistance(dxmath::Vector3, dxmath::Vector3);

	std::vector<MapTilePtr> GetPath(MapTilePtr, MapTilePtr);

	bool CanWalk(dxmath::Vector3);
	bool Within(MapTilePtr);
	MapTilePtr GetTileWithPosition(dxmath::Vector3);
	Vector3 GetNearestNeighbor(dxmath::Vector3);

	ID3D11DeviceContext1* context;
	std::vector<MapTilePtr> tiles;
	std::vector<MapTilePtr> ocuppiedTiles;
	int widthInTiles;
	int heightInTiles;
	const int tileSize = 1.f;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;
};

