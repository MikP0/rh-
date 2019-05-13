#include "pch.h"
#include "MapEdge.h"


MapEdge::MapEdge()
{
}

MapEdge::MapEdge(std::shared_ptr<MapTile> n1, std::shared_ptr<MapTile> n2, float cost)
{
	node1 = n1;
	node2 = n1;
	this->cost = cost;
}


MapEdge::~MapEdge()
{
}

//std::shared_ptr<MapEdge> MapEdge::Create(std::shared_ptr<MapTile> tile, std::shared_ptr<MapTile> neighbor)
//{
//	float edgeCost = CalculateCost(*tile, *neighbor);
//	if (edgeCost < 0.6f) {
//		return std::shared_ptr<MapEdge>(new MapEdge(tile, neighbor, edgeCost));
//	}
//	return nullptr;
//}
//
//float MapEdge::CalculateCost(MapTile n1, MapTile n2)
//{
//	float dx = abs(n1.worldPosition.x - n2.worldPosition.x);
//	float dy = abs(n1.worldPosition.y - n2.worldPosition.y);
//	float dz = abs(n1.worldPosition.z - n2.worldPosition.z);
//
//	float dxz = sqrt(pow(dx, 2) + pow(dz, 2));
//	float slope = dy / dxz;
//
//	return slope;
//}
