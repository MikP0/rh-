#pragma once
#include "pch.h"
#include "Terrain.h"
#include <map>
#include <unordered_set>
#include <queue>
#include <limits>

using namespace std;
using namespace DirectX::SimpleMath;

Terrain::Terrain()
{
}


Terrain::~Terrain()
{
}

void Terrain::InitTileMap(ID3D11DeviceContext1* context, vector<ColliderBasePtr> colliders)
{
	this->context = context;
	ResetTileMap();
	SetTilePositionsAndTypes();
	ConnectNeighboringTiles();
	//SetStaticObjects(colliders);
}

void Terrain::ResetTileMap()
{
	widthInTiles = 50;
	heightInTiles = 50;
	for (int i = 0; i < widthInTiles*heightInTiles; i++) {
		tiles.push_back(MapTilePtr(new MapTile()));
		tiles[i]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 0.f, tileSize), false, true);
	}
}

void Terrain::SetTilePositionsAndTypes()
{
	int beginW = -15;
	int beginH = 0;
	for (int i = 0; i < widthInTiles; i++) {
		beginH = 0;
		for (int j = 0; j < heightInTiles; j++) {			
			tiles[i *heightInTiles + j]->worldPosition = Vector3(beginW*1.f, 0.f, beginH*1.f);
			tiles[i *heightInTiles + j]->mapPosition = Vector2(i, j);
			//if (i*j % 15 < 10) {
			tiles[i *heightInTiles + j]->type = TileType::grass;
			tiles[i *heightInTiles + j]->walkable = true;

			beginH++;
			//}
			//else {
			//	tiles[i *heightInTiles + j]->type = TileType::fire;
			//	tiles[i *heightInTiles + j]->walkable = false;
			//	tiles[i *heightInTiles + j]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 0.f, tileSize));
			//}
			/*if ((i < 4 || i>5) && (j>2 && j<13)) {
				tiles[i *heightInTiles + j]->type = TileType::fire;
				tiles[i *heightInTiles + j]->walkable = false;
				tiles[i *heightInTiles + j]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 10.f, tileSize));
			}
			else {
				tiles[i *heightInTiles + j]->type = TileType::grass;
				tiles[i *heightInTiles + j]->walkable = true;
			}*/
		}
		beginW++;
	}
}

void Terrain::ConnectNeighboringTiles()
{
	for each (MapTilePtr tile in tiles)
	{
		Vector3 base = tile->worldPosition;
		int k = 0;
		for (int i = 1; i >= -1; i--) {
			for (int j = 1; j >= -1; j--) {
				MapTilePtr temp = this->GetTileWithPosition(base + Vector3(i, 0, j));
				if (temp == tile) {
					continue;
				}
				if (temp != nullptr) {
					if (temp->walkable) {
						tile->edges.push_back(shared_ptr<MapEdge>(new MapEdge(tile, temp, 0.f)));
					}
					else {
						tile->edges.push_back(shared_ptr<MapEdge>(new MapEdge(tile, temp, 1.f)));
					}
				}
				else {
					tile->edges.push_back(nullptr);
				}
				k++;
			}
		}
	}
}

void Terrain::SetStaticObjects(vector<ColliderBasePtr> colliders)
{
	typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
	typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
	for each (ColliderBasePtr collider in colliders)
	{
		if (collider->Type == AABB) {
			ColliderAABBptr colliderr = dynamic_pointer_cast<ColliderAABB>(collider);
			Vector3 center = colliderr->GetCenter();
			Vector3 a = center + colliderr->GetExtents();
			Vector3 b = center - colliderr->GetExtents();
			Vector3 temp1 = a;
			Vector3 temp2 = Vector3(temp1.x, temp1.y, b.z);
			for (temp1.x; temp1.x > b.x; temp1.x -= tileSize / 3.f) {
				temp2.x = temp1.x;
				this->MakeOcupied(temp1);
				this->MakeOcupied(temp2);
			}
			temp1 = a;
			temp2 = Vector3(b.x, temp1.y, temp1.z);
			for (temp1.z; temp1.z > b.z; temp1.z -= tileSize / 3.f) {
				temp2.z = temp1.z;
				this->MakeOcupied(temp1);
				this->MakeOcupied(temp2);
			}
			this->MakeOcupied(center);
			this->MakeOcupied(a);
			this->MakeOcupied(b);


			continue;
		}
		if (collider->Type == Sphere) {
			ColliderSpherePtr colliderr = dynamic_pointer_cast<ColliderSphere>(collider);
			this->MakeOcupied(colliderr->GetCenter());
			continue;
		}
	}
}

void Terrain::Draw(Camera camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roomTex)
{
	view = camera.GetViewMatrix();
	projection = camera.GetProjectionMatrix();
	tex = roomTex;
	for each (MapTilePtr tile in tiles)
	{
		if (tile->walkable) {
			tile->block->Draw(dxmath::Matrix::CreateTranslation(tile->worldPosition), camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::DimGray, roomTex.Get());
		}
		else {
			tile->block->Draw(dxmath::Matrix::CreateTranslation(tile->worldPosition), camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::Black, roomTex.Get());
		}
	}
}

void Terrain::MakeOcupied(dxmath::Vector3 position)
{
	MapTilePtr tile = this->GetTileWithPosition(position);	
	if (tile != nullptr)
	{
		tile->walkable = false;
		ocuppiedTiles.push_back(tile);
	}
}

void Terrain::Update(vector<ColliderBasePtr> colliders)
{
	typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
	typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
	this->ClearTiles();
	for each (ColliderBasePtr collider in colliders)
	{
		if (collider->Type == AABB) {
			ColliderAABBptr colliderr = dynamic_pointer_cast<ColliderAABB>(collider);
			Vector3 center = colliderr->GetCenter();
			Vector3 a = center + colliderr->GetExtents();
			Vector3 b = center - colliderr->GetExtents();
			Vector3 temp1 = a;
			Vector3 temp2 = Vector3(temp1.x, temp1.y, b.z);
			for (temp1.x; temp1.x > b.x; temp1.x -= tileSize / 3.f) {
				temp2.x = temp1.x;
				this->MakeOcupied(temp1);
				this->MakeOcupied(temp2);
			}
			temp1 = a;
			temp2 = Vector3(b.x, temp1.y, temp1.z);
			for (temp1.z; temp1.z > b.z; temp1.z -= tileSize / 3.f) {
				temp2.z = temp1.z;
				this->MakeOcupied(temp1);
				this->MakeOcupied(temp2);
			}
			this->MakeOcupied(center);
			this->MakeOcupied(a);
			this->MakeOcupied(b);


			continue;
		}
		if (collider->Type == Sphere) {
			ColliderSpherePtr colliderr = dynamic_pointer_cast<ColliderSphere>(collider);
			this->MakeOcupied(colliderr->GetCenter());
			continue;
		}
	}
}

void Terrain::ClearTiles() {
	for each (MapTilePtr tile in tiles)
	{
		tile->walkable = true;
	}
}

void Terrain::CoverTilesInRange(dxmath::Vector3 position, float range)
{
	MapTilePtr tile = GetTileWithPosition(position);
	if (tile != nullptr) {
		for each (shared_ptr<MapEdge> edge in tile->edges)
		{

		}
	}
}

bool Terrain::CanWalk(dxmath::Vector3 position)
{
	if ((position.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (position.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
		&& (position.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (position.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)))) {

		MapTilePtr tempPtr = this->GetTileWithPosition(position);
		if (tempPtr != nullptr) {
			if (!tempPtr->walkable && abs(dxmath::Vector3::Distance(tempPtr->worldPosition, position)) > (tileSize *sqrtf(2.f) / 2.f) - 0.07f) {
				return true;
			}
			return tempPtr->walkable;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool Terrain::Within(MapTilePtr tile)
{
	return (tile->worldPosition.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (tile->worldPosition.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
		&& (tile->worldPosition.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (tile->worldPosition.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)));

}

MapTilePtr Terrain::GetTileWithPosition(dxmath::Vector3 position)
{
	float x = position.x;
	float z = position.z;
	for each (MapTilePtr tile in tiles)
	{
		if (x < (tile->worldPosition.x + (tileSize / 2.f)) && x >(tile->worldPosition.x - (tileSize / 2.f))
			&& z < (tile->worldPosition.z + (tileSize / 2.f)) && z >(tile->worldPosition.z - (tileSize / 2.f))) {
			return tile;
		}
	}
	return nullptr;
}

Vector3 Terrain::GetNearestNeighbor(dxmath::Vector3 position)
{
	MapTilePtr tile = this->GetTileWithPosition(position);
	float distance = 100.f;
	MapTilePtr returnTile = nullptr;
	for each (shared_ptr<MapEdge> edge in tile->edges)
	{
		if (edge != nullptr && edge->node2->walkable) {
			float temp = sqrtf(pow(edge->node2->worldPosition.x - position.x, 2) + pow(edge->node2->worldPosition.y - position.y, 2) + pow(edge->node2->worldPosition.z - position.z, 2));
			if (temp < distance) {
				distance = temp;
				returnTile = edge->node2;
			}
		}
	}
	return returnTile->worldPosition;
}

Vector3 Terrain::FallBack(Vector3 current, Vector3 previous)
{
	Vector3 diff = previous - current;
	MapTilePtr result =nullptr;
	for (int i = 1; i < 15; i++) {
		result = this->GetTileWithPosition(current + (diff*i));
		if (result->walkable)
		{
			return result->worldPosition;
		}
	}
	return previous+(20*diff);
}

vector<MapTilePtr> Terrain::GetPath(MapTilePtr start, MapTilePtr goal)
{
	if (start == nullptr || goal == nullptr || start == goal || !this->Within(start) || !this->Within(goal)) {
		return vector<MapTilePtr>();
	}

	if (!start->walkable || !goal->walkable) {
		return vector<MapTilePtr>();
	}

	for each (MapTilePtr tile in tiles)
	{
		tile->f = std::numeric_limits<float>::max();
		tile->g = std::numeric_limits<float>::max();
	}

	struct classcomp {
		bool operator() (const MapTilePtr lhs, const MapTilePtr rhs) const
		{
			return lhs->g > rhs->g;
		}
	};

	auto cmp = [](MapTilePtr left, MapTilePtr right) { return left->f > right->f; };

	multimap<MapTilePtr, float, classcomp> open = multimap<MapTilePtr, float, classcomp>();
	unordered_set<MapTilePtr> closed = unordered_set<MapTilePtr>();

	start->g = 0;
	start->f = this->HexDistance(start->worldPosition, goal->worldPosition);

	open.insert(pair<MapTilePtr, float>(start, start->f));

	MapTilePtr current = nullptr;

	while (!open.empty() && current != goal) {
		multimap<MapTilePtr, float>::iterator iter = open.begin();
		current = iter->first;
		open.erase(iter);

		closed.insert(current);

		for (int i = 0; i < 8; i++) {
			shared_ptr<MapEdge> edge = current->edges[i];
			if (edge == nullptr) {
				continue;
			}
			MapTilePtr neighbor = edge->node2;
			float cost = current->g + edge->cost;
			if (!neighbor->walkable) {
				continue;
			}
			if (open.find(neighbor) != open.end() && cost < neighbor->g) {
				open.erase(neighbor);
			}
			if (closed.find(neighbor) != closed.end() && cost < neighbor->g) {
				closed.erase(neighbor);
			}
			if (open.find(neighbor) == open.end() && closed.find(neighbor) == closed.end()) {
				neighbor->g = cost;
				float f = cost + this->HexDistance(neighbor->worldPosition, goal->worldPosition); //heurystyka
				open.insert(pair<shared_ptr<MapTile>, float>(neighbor, f));
				neighbor->parent = current;
			}
		}
	}

	vector<MapTilePtr> path = vector<MapTilePtr>();

	while (current != start) {
		path.push_back(current);
		current = current->parent;
	}

	reverse(path.begin(), path.end());
	return path;
}

float Terrain::EuklideanDistance(Vector3 point1, Vector3 point2) {
	float dx = point1.x - point2.x;
	float dy = point1.y - point2.y;
	float dz = point1.z - point2.z;
	return sqrt(dx*dx + dy * dy + dz * dz);
}

float Terrain::ManhattanDistance(Vector3 start, Vector3 goal) {
	float dx = abs(start.x - goal.x);
	float dy = abs(start.y - goal.y);
	float dz = abs(start.z - goal.z);
	float h = dx + dy + dz;

	return h;
}

float Terrain::DiagonalDistance(Vector3 start, Vector3 goal) {
	float dx = abs(start.x - goal.x);
	float dy = abs(start.y - goal.y);
	float dz = abs(start.z - goal.z);
	float h = max(dx, dz);

	return h;
}

float Terrain::HexDistance(Vector3 start, Vector3 goal) {
	float dx = start.x - goal.x;
	float dy = start.z - goal.z;
	float dz = dx - dy;
	float h = max(abs(dx), max(abs(dy), abs(dz)));

	return h;
}
