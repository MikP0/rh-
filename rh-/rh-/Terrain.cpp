#pragma once
#include "pch.h"
#include "Terrain.h"
#include <map>
#include <unordered_set>

using namespace std;
using namespace DirectX::SimpleMath;

Terrain::Terrain()
{
}


Terrain::~Terrain()
{
}

void Terrain::InitTileMap(ID3D11DeviceContext1* context)
{
	this->context = context;
	ResetTileMap();
	SetTilePositionsAndTypes();
	ConnectNeighboringTiles();
}

void Terrain::ResetTileMap()
{
	widthInTiles = 15;
	heightInTiles = 15;
	for (int i = 0; i < widthInTiles*heightInTiles; i++) {
		tiles.push_back(shared_ptr<MapTile>(new MapTile()));
		tiles[i]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 0.f, tileSize), false, true);
	}
}

void Terrain::SetTilePositionsAndTypes()
{
	for (int i = 0; i < widthInTiles; i++) {
		for (int j = 0; j < heightInTiles; j++) {
			tiles[i *heightInTiles + j]->worldPosition = Vector3(i*1.f, 0.f, j*1.f);
			tiles[i *heightInTiles + j]->mapPosition = Vector2(i, j);
			if (i*j % 3 < 2) {
				tiles[i *heightInTiles + j]->type = TileType::grass;
				tiles[i *heightInTiles + j]->walkable = true;
			}
			else {
				tiles[i *heightInTiles + j]->type = TileType::fire;
				tiles[i *heightInTiles + j]->walkable = false;
				tiles[i *heightInTiles + j]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 0.f, tileSize));
			}
		}
	}
}

void Terrain::ConnectNeighboringTiles()
{
	for each (shared_ptr<MapTile> tile in tiles)
	{
		Vector3 base = tile->worldPosition;
		int k = 0;
		for (int i = 1; i >= -1; i--) {
			for (int j = 1; j >= -1; j--) {
				shared_ptr<MapTile> temp = this->GetTileWithPosition(base + Vector3(i, 0, j));
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

void Terrain::Draw(Camera camera, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roomTex)
{
	view = camera.GetViewMatrix();
	projection = camera.GetProjectionMatrix();
	tex = roomTex;
	for each (shared_ptr<MapTile> tile in tiles)
	{
		if (tile->walkable) {
			tile->block->Draw(dxmath::Matrix::CreateTranslation(tile->worldPosition), camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::Green, roomTex.Get());
		}
		else {
			tile->block->Draw(dxmath::Matrix::CreateTranslation(tile->worldPosition), camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::Red, roomTex.Get());
		}
	}
}

bool Terrain::CanWalk(dxmath::Vector3 position)
{
	if ((position.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (position.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
		&& (position.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (position.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)))) {

		shared_ptr<MapTile> tempPtr = this->GetTileWithPosition(position);
		if (tempPtr != nullptr) {
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

bool Terrain::Within(shared_ptr<MapTile> tile)
{
	return (tile->worldPosition.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (tile->worldPosition.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
		&& (tile->worldPosition.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (tile->worldPosition.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)));

}

shared_ptr<MapTile> Terrain::GetTileWithPosition(dxmath::Vector3 position)
{
	float x = position.x;
	float z = position.z;
	for each (shared_ptr<MapTile> tile in tiles)
	{
		if (x < (tile->worldPosition.x + (tileSize / 2.f)) && x >(tile->worldPosition.x - (tileSize / 2.f))
			&& z < (tile->worldPosition.z + (tileSize / 2.f)) && z >(tile->worldPosition.z - (tileSize / 2.f))) {
			return tile;
		}
	}
	return nullptr;
}

vector<shared_ptr<MapTile>> Terrain::GetPath(shared_ptr<MapTile> start, shared_ptr<MapTile> goal)
{
	if (start == nullptr || goal == nullptr || start == goal || !this->Within(start) || !this->Within(goal)) {
		return vector<shared_ptr<MapTile>>();
	}

	if (!start->walkable || !goal->walkable) {
		return vector<shared_ptr<MapTile>>();
	}

	for each (shared_ptr<MapTile> tile in tiles)
	{
		tile->f = 0;
		tile->g = 0;
	}

	struct classcomp {
		bool operator() (const shared_ptr<MapTile> lhs, const shared_ptr<MapTile> rhs) const
		{
			return lhs->f < rhs->f;
		}
	};

	multimap<shared_ptr<MapTile>, float, classcomp> open = multimap<shared_ptr<MapTile>, float, classcomp>();
	unordered_set<shared_ptr<MapTile>> closed = unordered_set<shared_ptr<MapTile>>();

	start->g = 0;
	start->f = this->EuklideanDistance(start->worldPosition, goal->worldPosition);

	open.insert(pair<shared_ptr<MapTile>, float>(start, start->f));

	shared_ptr<MapTile> current = nullptr;

	while (!open.empty() && current != goal) {
		multimap<shared_ptr<MapTile>, float>::iterator iter = open.begin();
		current = iter->first;
		open.erase(iter);
		closed.insert(current);

		for (int i = 0; i < 8; i++) {
			shared_ptr<MapEdge> edge = current->edges[i];
			if (edge == nullptr) {
				continue;
			}
			shared_ptr<MapTile> neighbor = edge->node2;
			float cost = current->g + edge->cost;

			if (open.find(neighbor) != open.end() && cost < neighbor->g) {
				open.erase(neighbor);
			}
			if (closed.find(neighbor) != closed.end() && cost < neighbor->g) {
				closed.erase(neighbor);
			}
			if (open.find(neighbor) == open.end() && closed.find(neighbor) == closed.end()) {
				neighbor->g = cost;
				float f = cost + this->EuklideanDistance(neighbor->worldPosition, goal->worldPosition); //heurystyka
				open.insert(pair<shared_ptr<MapTile>, float>(neighbor, f));
				neighbor->parent = current;
			}
		}
	}

	vector<shared_ptr<MapTile>> path = vector<shared_ptr<MapTile>>();

	while (current != start) {
		path.push_back(current);
		current = current->parent;
	}

	reverse(path.begin(), path.end());
	//for each (shared_ptr<MapTile> tile in path)
	//{
		//tiles[0]->block->Draw(dxmath::Matrix::CreateTranslation(tiles[0]->worldPosition+ Vector3(0.f, 3.f, 0.f)), view, projection, Colors::Blue, tex.Get());
		//tile->block->GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 5.f, tileSize), false, true);
	//}
	return path;	
}

float Terrain::EuklideanDistance(Vector3 point1, Vector3 point2) {
	float dx = point1.x - point2.x;
	float dy = point1.y - point2.y;
	float dz = point1.z - point2.z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}
