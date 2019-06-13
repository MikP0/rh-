#pragma once
#include "pch.h"

class MapTile; 

class MapEdge
{
public:
	MapEdge();
	MapEdge(std::shared_ptr<MapTile>, std::shared_ptr<MapTile>, float);
	virtual ~MapEdge();

	std::shared_ptr<MapTile> node1;
	std::shared_ptr<MapTile> node2;
	float cost;
};

