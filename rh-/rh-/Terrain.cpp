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
	ResetTileMap();
	//SetTilePositionsAndTypes();
	//CalculateWalkability();
	//ConnectNeighboringTiles();
	//CreateTileSets();
	//tiles[0]->block = GeometricPrimitive::CreateBox(context, XMFLOAT3(1.f, 0.f, 1.f), false, true);
}

void Terrain::ResetTileMap()
{
	//widthInTiles = 360 / tileSize;
	//heightInTiles = 360 / tileSize;
	//for (int i = 0; i < widthInTiles*heightInTiles; i++) {
		//tiles.push_back(std::shared_ptr<MapTile>(new MapTile()));
	//}
}

void Terrain::SetTilePositionsAndTypes()
{
	//for (int y = 0; y < heightInTiles; y++) {
	//	for (int x = 0; x < widthInTiles; x++) {
	//		std::shared_ptr<MapTile> tile = this->GetTile(x, y);
	//		tile->mapPosition = Vector2(x, y);
	//		// Calculate world position of tile center
	//		float worldX = (x * 0.01f * tileSize) + (0.01f * tileSize / 2) - (100.f / 2);
	//		float worldZ = (-y * 0.01f * tileSize) - (0.01f * tileSize / 2) + (100.f / 2);
	//		float height = Height(worldX, worldZ);
	//		tile->worldPosition = Vector3(worldX, height, worldZ);

	//		// Set tile type
	//		if (tile->height < HeightMap.MaxHeight*(0.05f)) {
	//			tile->type = fire;
	//		}
	//		else if (tile->height < HeightMap.MaxHeight*(0.4f)) {
	//			tile->type = grass;
	//		}
	//		else {
	//			tile->type = snow;
	//		}
	//	}
	//}
}

void Terrain::CalculateWalkability()
{
}

void Terrain::ConnectNeighboringTiles()
{
}

void Terrain::CreateTileSets()
{
}

void Terrain::Draw(Camera camera, ID3D11Device1* device)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;
	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"roomtexture.dds", nullptr, m_roomTex.ReleaseAndGetAddressOf()));
	//tiles[0]->block->Draw(Matrix::Identity, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_roomTex.Get());
}

std::shared_ptr<MapTile> Terrain::GetTile(int x, int y)
{
	if (tiles.empty()) {
		return nullptr;
	}
	else {
		return tiles[x + y * heightInTiles];
	}
}

bool Terrain::Within(dxmath::Vector2 point)
{
	return point.x >= 0 && point.x < widthInTiles && point.y >= 0 && point.y < heightInTiles;
}
