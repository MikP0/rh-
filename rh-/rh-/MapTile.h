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

	dxmath::Vector3 worldPosition;
	dxmath::Vector2 mapPosition;
	//float height;	
	TileType type;
	bool walkable;
	//int set;
	float f;
	float g;
	std::shared_ptr<MapTile> parent;
	/*0 - NW, 1 - N, 2 - NE, 3 - W, 4 - E, 5 - SW, 6 - S, 7 - SE */
	std::vector<std::shared_ptr<MapEdge>> edges;
	std::shared_ptr<DirectX::GeometricPrimitive> block;
private:
};

