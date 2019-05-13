#include "pch.h"
#include "Terrain.h"

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
		tiles.push_back(std::shared_ptr<MapTile>(new MapTile()));
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
				tiles[i *heightInTiles + j]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(tileSize, 3.f, tileSize));
			}
		}
	}
}

void Terrain::ConnectNeighboringTiles()
{
	for each (std::shared_ptr<MapTile> tile in tiles)
	{
		Vector3 base = tile->worldPosition;
		int k = 0;
		for (int i = 1; i >= -1; i--) {
			for (int j = 1; j >= -1; j--) {
				std::shared_ptr<MapTile> temp = this->GetTileWithPosition(base + Vector3(i, 0, j));
				if (temp != nullptr) {
					if (temp->walkable) {
						tile->edges.push_back(std::shared_ptr<MapEdge>(new MapEdge(tile, temp, 0.f)));
					}
					else {
						tile->edges.push_back(std::shared_ptr<MapEdge>(new MapEdge(tile, temp, 1.f)));
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
	for each (std::shared_ptr<MapTile> tile in tiles)
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

		std::shared_ptr<MapTile> tempPtr = this->GetTileWithPosition(position);
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

std::shared_ptr<MapTile> Terrain::GetTileWithPosition(dxmath::Vector3 position)
{
	float x = position.x;
	float z = position.z;
	for each (std::shared_ptr<MapTile> tile in tiles)
	{
		if (x < (tile->worldPosition.x + (tileSize / 2.f)) && x >(tile->worldPosition.x - (tileSize / 2.f))
			&& z < (tile->worldPosition.z + (tileSize / 2.f)) && z >(tile->worldPosition.z - (tileSize / 2.f))) {
			return tile;
		}
	}
	return nullptr;
}
