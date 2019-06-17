#include "pch.h"
#include "MapEdge.h"


MapEdge::MapEdge()
{
}

MapEdge::MapEdge(std::shared_ptr<MapTile> n1, std::shared_ptr<MapTile> n2, float cost)
{
	node1 = n1;
	node2 = n2;
	this->cost = cost;
}


MapEdge::~MapEdge()
{
}
