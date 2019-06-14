#pragma once

#include <DirectXColors.h>
#include "DeviceResources.h"
#include "OctTree.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef shared_ptr<ColliderAABB> ColliderAABBptr;
//typedef shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef shared_ptr<OctTree> OctTreePtr;

class DebugDraw
{
public:
	DebugDraw(ID3D11Device1* _device, ID3D11DeviceContext1* _context);
	~DebugDraw();

	void DrawBox(XMFLOAT3 &dimensions, Vector3 &position, XMMATRIX &view, XMMATRIX &projection, XMVECTORF32 color);
	void DrawSphere(float &diameter, Vector3 &position, XMMATRIX &view, XMMATRIX &projection, XMVECTORF32 color);
	void DrawOctTree(OctTreePtr octTree, XMMATRIX &view, XMMATRIX &projection, bool &showTreeRegion);

private:
	ID3D11Device1* _device;
	ID3D11DeviceContext1* _context;
};

