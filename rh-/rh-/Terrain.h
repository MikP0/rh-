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

	void InitTileMap(ID3D11DeviceContext1*, vector<ColliderBasePtr>);
	void ResetTileMap();
	void SetTilePositionsAndTypes();
	void ConnectNeighboringTiles();
	void SetStaticObjects(vector<ColliderBasePtr>);
	void MakeOcupied(dxmath::Vector3);

	void Draw(Camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>);
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
	MapTilePtr GetTileWithPosition(Vector3);
	Vector3 GetNearestNeighbor(Vector3);
	Vector3 FallBack(Vector3, Vector3);

	ID3D11DeviceContext1* context;
	std::vector<MapTilePtr> tiles;
	std::vector<MapTilePtr> ocuppiedTiles;
	int widthInTiles;
	int heightInTiles;
	const float tileSize = 1.f;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;
};

