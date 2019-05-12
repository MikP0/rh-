#pragma once
#include "pch.h"
#include "MapEdge.h"

namespace dxmath = DirectX::SimpleMath;

enum TileType {
	grass,
	snow,
	fire
};

class MapTile : public std::enable_shared_from_this<MapTile>
{
public:
	MapTile();
	virtual ~MapTile();
	void Create(ID3D11DeviceContext1 *);

	/*dxmath::Vector3 worldPosition;
	float height;
	dxmath::Vector2 mapPosition;
	TileType type;
	bool walkable;
	int set;
	float f;
	float g;
	std::shared_ptr<MapTile> parent;*/
	//std::shared_ptr<MapEdge> edges;
	std::shared_ptr<DirectX::GeometricPrimitive> block;
	//std::shared_ptr<DirectX::Model> block;
private:
};

