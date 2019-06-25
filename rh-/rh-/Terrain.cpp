#pragma once
#include "pch.h"
#include "Terrain.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"

#include <map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;
using namespace DirectX::SimpleMath;

Terrain::Terrain()
{
}


Terrain::~Terrain()
{
}

void Terrain::Initialize(ID3D11DeviceContext1* context, ID3D11Device1* device, shared_ptr<Entity> player)
{
	this->context = context;
	playerEntity = player;

	m_states = std::make_unique<CommonStates>(device);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t byteCodeLength;

		m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		DX::ThrowIfFailed(
			device->CreateInputLayout(
				VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
				shaderByteCode, byteCodeLength,
				m_inputLayout.ReleaseAndGetAddressOf()));
	}

	ResetTileMap();
	SetTilesPosition(-50, 0);
	CreateEdges();
}

void Terrain::ResetTileMap()
{
	tiles.clear();
	for (int i = 0; i < widthInTiles*heightInTiles; i++) {
		tiles.push_back(MapTilePtr(new MapTile()));
	}
}

void Terrain::SetTilesPosition(int beginWidth, int beginHeight)
{
	int beginW = beginWidth;
	int beginH = beginHeight;
	for (int i = 0; i < widthInTiles; i++) {
		beginH = beginHeight;
		for (int j = 0; j < heightInTiles; j++) {
			tiles[i *heightInTiles + j]->worldPosition = Vector3(beginW*tileSize, 0.f, beginH*tileSize);
			tiles[i *heightInTiles + j]->mapPosition = Vector2(i, j);
			tilesMap[tiles[i *heightInTiles + j]->mapPosition] = tiles[i *heightInTiles + j];
			beginH++;
		}
		beginW++;
	}
}

void Terrain::CreateEdges() {
	for each (MapTilePtr tile in tiles)
	{
		tile->edges.resize(8);
		Vector2 basePosition = tile->mapPosition;
		int k = 0;
		for (int i = 1; i >= -1; i--)
		{
			for (int j = 1; j >= -1; j--)
			{
				MapTilePtr temp = GetTileFromMap(basePosition + Vector2(i, j));
				if (temp == tile)
				{
					continue;
				}
				else if (temp != nullptr)
				{
					temp->edges.resize(8);
					shared_ptr<MapEdge> newEdge = shared_ptr<MapEdge>(new MapEdge(tile, temp, 0.f));
					tile->edges[k] = newEdge;
					temp->edges[7 - k] = newEdge;
					k++;

				}
				else
				{
					tile->edges.push_back(nullptr);
					k++;
				}
			}
		}
	}
}

void Terrain::CreateWorld(vector<shared_ptr<PhysicsComponent>> colliders)
{
	colliders.begin();
	typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
	for each (auto collider in colliders)
	{
		if (collider->ColliderBounding->Type == AABB) {
			ColliderAABBptr colliderr = dynamic_pointer_cast<ColliderAABB>(collider->ColliderBounding);
			Vector3 center = colliderr->GetCenter();
			Vector3 a = center + colliderr->GetExtents() - Vector3(0.1f, 0.f, 0.1f);
			Vector3 b = center - colliderr->GetExtents() + Vector3(0.1f, 0.f, 0.1f);

			MapTilePtr tileA = GetTileWithPosition(a);
			MapTilePtr tileB = GetTileWithPosition(b);
			if (tileA != nullptr && tileB != nullptr && tileA != tileB)
			{
				Vector2 base = tileB->mapPosition;
				int diffW = tileA->mapPosition.x - tileB->mapPosition.x;
				int diffH = tileA->mapPosition.y - tileB->mapPosition.y;
				for (int i = 0; i <= diffW; i++)
				{
					for (int j = 0; j <= diffH; j++)
					{
						MapTilePtr temp = GetTileFromMap(base + Vector2(i, j));
						if (!collider->IsTriggered)
						{
							MakeOcupied(temp);
						}
						else
						{
							temp->type = TileType::world;
							if (collider->GetParent()->GetTag() == Tags::PLAYER || collider->GetParent()->GetTag() == Tags::ENEMY) {
								characters.push_back(collider);
							}
						}
					}
				}
			}
			else
			{
				if (tileA != nullptr)
				{
					if (!collider->IsTriggered)
					{
						MakeOcupied(tileA);
					}
					else
					{
						tileA->type = TileType::world;
						if (collider->GetParent()->GetTag() == Tags::PLAYER || collider->GetParent()->GetTag() == Tags::ENEMY) {
							characters.push_back(collider);
						}
					}
				}
			}
		}
	}
	std::vector<MapTilePtr> copied = tiles;

	for each (MapTilePtr tile in copied)
	{
		if (tile->type == TileType::empty) {
			tilesMap.erase(tile->mapPosition);
			vector<MapTilePtr>::iterator position = std::find(tiles.begin(), tiles.end(), tile);
			if (position != tiles.end())
				tiles.erase(position);
		}
	}
}

void Terrain::Draw(Camera camera)
{
	m_effect->SetView(camera.GetViewMatrix());
	m_effect->SetProjection(camera.GetProjectionMatrix());

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullNone());

	m_effect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	MapTilePtr playerTile = GetTileWithPosition(playerEntity->GetTransform()->GetPosition());
	Vector2 posOrigin = playerTile->mapPosition;
	Vector3 pos = playerTile->worldPosition;
	XMVECTOR color = Colors::WhiteSmoke;

	m_batch->Begin();

	DrawRange(posOrigin, 25, 25, Colors::WhiteSmoke);
	FixRendering();

	if (playerEntity->GetComponent<PlayerComponent>()->vampireAbility == 1)
	{
		color = Colors::IndianRed;
		DrawRange(posOrigin, 11, 11, color);
		FixRendering();
	}
	if (playerEntity->GetComponent<PlayerComponent>()->vampireAbility == 2)
	{
		color = Colors::IndianRed;
		for each (shared_ptr<PhysicsComponent> var in characters)
		{
			if (var != nullptr && var->_isEnabled)
			{
				MapTilePtr tempTile = GetTileWithPosition(var->GetParent()->GetTransform()->GetPosition());
				if (tempTile != playerTile && XMVector3NearEqual(tempTile->worldPosition, playerTile->worldPosition, Vector3(1.f, 1.f, 1.f)))
				{
					FillTile(tempTile->worldPosition, color);
				}
			}
			else
			{
				vector<shared_ptr<PhysicsComponent>>::iterator position = std::find(characters.begin(), characters.end(), var);
				if (position != characters.end())
					characters.erase(position);
			}
		}
	}
	if (playerEntity->GetComponent<PlayerComponent>()->vampireAbility == 3)
	{
		color = Colors::IndianRed;
		for each (shared_ptr<PhysicsComponent> var in characters)
		{
			if (var != nullptr && var->_isEnabled)
			{
				MapTilePtr tempTile = GetTileWithPosition(var->GetParent()->GetTransform()->GetPosition());
				FillTile(tempTile->worldPosition, color);
			}
			else
			{
				vector<shared_ptr<PhysicsComponent>>::iterator position = std::find(characters.begin(), characters.end(), var);
				if (position != characters.end())
					characters.erase(position);
			}
		}
	}
	if (playerEntity->GetComponent<PlayerComponent>()->vampireAbility == 4)
	{
		color = Colors::IndianRed;
		/*for (int i = -4 / 2; i <= 4 / 2; i++)
		{
			for (int j = -4 / 2; j <= 4 / 2; j++) {
				MapTilePtr tile = GetTileFromMap(posOrigin + Vector2(i, j));
				if (tile != nullptr && tile->walkable)
				{
					VertexPositionColor v1(tile->worldPosition + Vector3(tileSize / 2.f, 0.f, tileSize / 2.f), color);
					VertexPositionColor v2(tile->worldPosition + Vector3(tileSize / 2.f, 0.f, -tileSize / 2.f), color);
					VertexPositionColor v3(tile->worldPosition + Vector3(-tileSize / 2.f, 0.f, -tileSize / 2.f), color);
					VertexPositionColor v4(tile->worldPosition + Vector3(-tileSize / 2.f, 0.f, tileSize / 2.f), color);

					m_batch->DrawQuad(v1, v2, v3, v4);
				}
			}
		}*/
		DrawRange(posOrigin, 4, 4, color);
		FixRendering();
	}
	m_batch->End();
}

void Terrain::DrawRange(Vector2 centerPosition, int width, int height, XMVECTOR color)
{
	for (int i = -width / 2; i <= width / 2; i++)
	{
		for (int j = -height / 2; j <= height / 2; j++) {
			MapTilePtr tile = GetTileFromMap(centerPosition + Vector2(i, j));
			if (tile != nullptr && tile->walkable)
			{
				VertexPositionColor verts[5];
				XMStoreFloat3(&verts[0].position, tile->worldPosition + Vector3(tileSize / 2.f, 0.f, tileSize / 2.f));
				XMStoreFloat3(&verts[1].position, tile->worldPosition + Vector3(tileSize / 2.f, 0.f, -tileSize / 2.f));
				XMStoreFloat3(&verts[2].position, tile->worldPosition + Vector3(-tileSize / 2.f, 0.f, -tileSize / 2.f));
				XMStoreFloat3(&verts[3].position, tile->worldPosition + Vector3(-tileSize / 2.f, 0.f, tileSize / 2.f));
				XMStoreFloat3(&verts[4].position, tile->worldPosition + Vector3(tileSize / 2.f, 0.f, tileSize / 2.f));

				XMStoreFloat4(&verts[0].color, color);
				XMStoreFloat4(&verts[1].color, color);
				XMStoreFloat4(&verts[2].color, color);
				XMStoreFloat4(&verts[3].color, color);
				XMStoreFloat4(&verts[4].color, color);

				m_batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 5);
			}
		}
	}
}

void Terrain::FillTile(Vector3 position, XMVECTOR color)
{
	VertexPositionColor v1(position + Vector3(tileSize / 2.f, 0.f, tileSize / 2.f), color);
	VertexPositionColor v2(position + Vector3(tileSize / 2.f, 0.f, -tileSize / 2.f), color);
	VertexPositionColor v3(position + Vector3(-tileSize / 2.f, 0.f, -tileSize / 2.f), color);
	VertexPositionColor v4(position + Vector3(-tileSize / 2.f, 0.f, tileSize / 2.f), color);

	m_batch->DrawQuad(v1, v2, v3, v4);

}

void Terrain::FixRendering()
{
	VertexPositionColor v1(Vector3(0, 0, 0) + Vector3(tileSize / 2.f, 0.f, tileSize / 2.f), XMVECTOR{ 1.0f, 1.0f, 1.0f, 0.0f });
	VertexPositionColor v2(Vector3(0, 0, 0) + Vector3(tileSize / 2.f, 0.f, -tileSize / 2.f), XMVECTOR{ 1.0f, 1.0f, 1.0f, 0.0f });
	VertexPositionColor v3(Vector3(0, 0, 0) + Vector3(-tileSize / 2.f, 0.f, -tileSize / 2.f), XMVECTOR{ 1.0f, 1.0f, 1.0f, 0.0f });
	VertexPositionColor v4(Vector3(0, 0, 0) + Vector3(-tileSize / 2.f, 0.f, tileSize / 2.f), XMVECTOR{ 1.0f, 1.0f, 1.0f, 0.0f });

	m_batch->DrawQuad(v1, v2, v3, v4);
}

void Terrain::MakeOcupied(MapTilePtr tile)
{
	if (tile != nullptr)
	{
		tile->walkable = false;
		tile->type = TileType::staticCollider;
		for each (shared_ptr<MapEdge> edge in tile->edges)
		{
			if (edge != nullptr) {
				edge->cost = 1.f;
			}
		}
	}
}
void Terrain::OccupyTile(MapTilePtr tile)
{
	if (tile != nullptr)
	{
		tile->walkable = false;
		for each (shared_ptr<MapEdge> edge in tile->edges)
		{
			if (edge != nullptr) {
				edge->cost = 1.f;
			}
		}
	}
}
void Terrain::ReleaseTile(MapTilePtr tile)
{
	if (tile != nullptr)
	{
		tile->walkable = true;
		for each (shared_ptr<MapEdge> edge in tile->edges)
		{
			if (edge != nullptr) {
				edge->cost = 0.f;
			}
		}
	}
}

bool Terrain::CanWalk(dxmath::Vector3 position)
{
	//if ((position.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (position.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
	//	&& (position.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (position.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)))) {

	MapTilePtr tempPtr = this->GetTileWithPosition(position);
	if (tempPtr != nullptr)
	{
		if (!tempPtr->walkable && abs(dxmath::Vector3::Distance(tempPtr->worldPosition, position)) > (tileSize *sqrtf(2.f) / 2.f) - 0.05f) {
			return true;
		}

		return tempPtr->walkable;
	}
	//}
	return false;
}

bool Terrain::CanMove(dxmath::Vector3 position, shared_ptr<PhysicsComponent> enemy)
{
	MapTilePtr tempPtr = this->GetTileWithPosition(position);
	if (tempPtr != nullptr)
	{
		/*for each (shared_ptr<PhysicsComponent> character in characters)
		{
			if (character != enemy && tempPtr == GetTileWithPosition(character->GetParent()->GetTransform()->GetPosition()))
			{
				return 2;
			}
		}*/
		if (tempPtr == GetTileWithPosition(enemy->GetParent()->GetTransform()->GetPosition()))
		{
			return true;
		}
		if (!tempPtr->walkable && abs(dxmath::Vector3::Distance(tempPtr->worldPosition, position)) > (tileSize *sqrtf(2.f) / 2.f) - 0.05f) {
			return true;
		}
		return (tempPtr->walkable) ? 1 : 0;
	}
	return false;
}

bool Terrain::Within(MapTilePtr tile)
{
	return (tile->worldPosition.x > (tiles.front()->worldPosition.x - (tileSize / 2.f))) && (tile->worldPosition.x < (tiles.back()->worldPosition.x + (tileSize / 2.f)))
		&& (tile->worldPosition.z > (tiles.front()->worldPosition.z - (tileSize / 2.f))) && (tile->worldPosition.z < (tiles.back()->worldPosition.z + (tileSize / 2.f)));

}

bool Terrain::IsTileOccupied(MapTilePtr tile, shared_ptr<PhysicsComponent> object)
{
	for each (shared_ptr<PhysicsComponent> character in characters)
	{
		if (character != object) {
			if (tile == GetTileWithPosition(character->GetParent()->GetTransform()->GetPosition())) {
				return true;
			}
		}
	}
	return false;
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

MapTilePtr Terrain::GetTileFromMap(Vector2 position)
{
	map<Vector2, MapTilePtr>::iterator it = tilesMap.find(position);
	if (it != tilesMap.end())
	{
		return tilesMap.at(position);
	}
	else
	{
		return nullptr;
	}
}

Vector3 Terrain::GetNearestNeighbor(dxmath::Vector3 position, MapTilePtr dest, shared_ptr<PhysicsComponent> enemy)
{
	MapTilePtr origin = GetTileWithPosition(position);
	float distance = 100.f;
	MapTilePtr returnTile = nullptr;
	for (int i = 0; i < 8; i++)
	{
		shared_ptr<MapEdge> edge = dest->edges[i];
		if (edge != nullptr)
		{
			if (edge->node1 != dest && edge->node1->walkable && edge->node1 != origin)
			{
				float temp = sqrtf(pow(edge->node1->worldPosition.x - position.x, 2) + pow(edge->node1->worldPosition.y - position.y, 2) + pow(edge->node1->worldPosition.z - position.z, 2));
				if (temp < distance) {
					distance = temp;
					returnTile = edge->node1;
				}
			}
			if (edge->node2 != dest && edge->node2->walkable && edge->node2 != origin)
			{
				float temp = sqrtf(pow(edge->node2->worldPosition.x - position.x, 2) + pow(edge->node2->worldPosition.y - position.y, 2) + pow(edge->node2->worldPosition.z - position.z, 2));
				if (temp < distance) {
					distance = temp;
					returnTile = edge->node2;
				}
			}
		}
	}
	return returnTile->worldPosition;
}

Vector3 Terrain::FallBack(Vector3 current, Vector3 previous)
{
	Vector3 diff = previous - current;
	MapTilePtr result = nullptr;
	for (int i = 1; i < 15; i++) {
		result = this->GetTileWithPosition(current + (diff*i));
		if (result->walkable)
		{
			return result->worldPosition;
		}
	}
	return previous + (20 * diff);
}

vector<MapTilePtr> Terrain::GetPath(MapTilePtr start, MapTilePtr goal)
{
	if (start == nullptr || goal == nullptr || start == goal) //|| !this->Within(start) || !this->Within(goal)) 
	{
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
			MapTilePtr neighbor = edge->node1 == current ? edge->node2 : edge->node1;
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
