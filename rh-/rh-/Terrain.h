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

	void Initialize(ID3D11DeviceContext1*, ID3D11Device1*, shared_ptr<Entity>);
	void ResetTileMap();
	void SetTilesPosition(int, int);
	void CreateEdges();

	void CreateWorld(vector<shared_ptr<PhysicsComponent>>);
	void MakeOcupied(MapTilePtr);
	void ReleaseTile(MapTilePtr);
	void OccupyTile(MapTilePtr);

	void Draw(Camera);
	void DrawRange(Vector2, int, int, XMVECTOR);
	void FillTile(Vector3, XMVECTOR);
	void FixRendering();

	vector<MapTilePtr> GetPath(MapTilePtr, MapTilePtr);
	float EuklideanDistance(dxmath::Vector3, dxmath::Vector3);
	float ManhattanDistance(dxmath::Vector3, dxmath::Vector3);
	float DiagonalDistance(dxmath::Vector3, dxmath::Vector3);
	float HexDistance(dxmath::Vector3, dxmath::Vector3);

	bool CanWalk(dxmath::Vector3);
	bool CanMove(dxmath::Vector3, shared_ptr<PhysicsComponent>);
	bool Within(MapTilePtr);
	bool IsTileOccupied(MapTilePtr, shared_ptr<PhysicsComponent>);
	MapTilePtr GetTileWithPosition(Vector3);
	MapTilePtr GetTileFromMap(Vector2);
	Vector3 GetNearestNeighbor(Vector3, MapTilePtr, shared_ptr<PhysicsComponent>);
	Vector3 FallBack(Vector3, Vector3);


	const int widthInTiles = 200;
	const int heightInTiles = 116;
	const float tileSize = 1.f;

	vector<MapTilePtr> tiles;
	map<Vector2, MapTilePtr> tilesMap;	
	shared_ptr<Entity> playerEntity;
	vector<shared_ptr<PhysicsComponent>> characters;

	ID3D11DeviceContext1* context;
	shared_ptr<DirectX::CommonStates> m_states;
	unique_ptr<DirectX::BasicEffect> m_effect;
	unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;	
};

