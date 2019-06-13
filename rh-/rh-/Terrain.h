#pragma once
#include "pch.h"
#include "MapTile.h"
#include "Camera.h"
#include "ColliderTypes.h"
#include "PhysicsComponent.h"

using namespace std;

typedef shared_ptr<MapTile> MapTilePtr;
typedef shared_ptr<ColliderBase> ColliderBasePtr;

class Terrain
{
public:
	Terrain();
	virtual ~Terrain();

	void Initialize(ID3D11DeviceContext1*);
	void ResetTileMap();
	void SetTilesPosition(int, int);
	void ConnectNeighboringTiles();
	void SetStaticObjects(vector<shared_ptr<PhysicsComponent>>);	

	void Draw(Camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>);
	void Update(vector<ColliderBasePtr>);
	void ClearTiles();

	vector<MapTilePtr> GetPath(MapTilePtr, MapTilePtr);
	float EuklideanDistance(dxmath::Vector3, dxmath::Vector3);
	float ManhattanDistance(dxmath::Vector3, dxmath::Vector3);
	float DiagonalDistance(dxmath::Vector3, dxmath::Vector3);
	float HexDistance(dxmath::Vector3, dxmath::Vector3);

	void MakeOcupied(dxmath::Vector3);
	bool CanWalk(dxmath::Vector3);
	bool CanMove(dxmath::Vector3, dxmath::Vector3);
	bool Within(MapTilePtr);
	MapTilePtr GetTileWithPosition(Vector3);
	Vector3 GetNearestNeighbor(Vector3);
	Vector3 FallBack(Vector3, Vector3);



	const int widthInTiles = 50;
	const int heightInTiles = 50;
	const float tileSize = 1.f;
	vector<MapTilePtr> tiles;
	
	vector<MapTilePtr> ocuppiedTiles;
	vector<shared_ptr<PhysicsComponent>> characters;

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tex;
	ID3D11DeviceContext1* context;
};

