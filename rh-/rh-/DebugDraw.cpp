#include "pch.h"
#include "DebugDraw.h"


DebugDraw::DebugDraw(ID3D11Device1* device, ID3D11DeviceContext1* context)
{
	_device = device;
	_context = context;
}


DebugDraw::~DebugDraw()
{
}

void DebugDraw::DrawBox(XMFLOAT3 &dimensions, Vector3 &position, XMMATRIX &view, XMMATRIX &projection, XMVECTORF32 color)
{
	Matrix worldMatrix = Matrix::CreateTranslation(position);
	unique_ptr<DirectX::GeometricPrimitive> primitive = 
		GeometricPrimitive::CreateBox(_context, dimensions, false, true);
	primitive->Draw(worldMatrix, view, projection, color, nullptr, true);
}

void DebugDraw::DrawSphere(float &diameter, Vector3 &position, XMMATRIX &view, XMMATRIX &projection, XMVECTORF32 color)
{
	Matrix worldMatrix = Matrix::CreateTranslation(position);
	unique_ptr<DirectX::GeometricPrimitive> primitive =
		GeometricPrimitive::CreateSphere(_context, diameter, 16, false, true);
	primitive->Draw(worldMatrix, view, projection, color, nullptr, true);
}

void DebugDraw::DrawOctTree(OctTreePtr octTree, XMMATRIX &view, XMMATRIX &projection, bool &showTreeRegion)
{
	if (octTree == nullptr)
		return;

	if (showTreeRegion)
	{
		XMFLOAT3 dimensions = octTree->Region->GetExtents() * 2.0f;
		DrawBox(dimensions, octTree->Region->GetCenter(), view, projection, Colors::Yellow);
	}

	list<PhysicsComponentPtr> nodeObjects = octTree->NodeObjects;

	for each(PhysicsComponentPtr object in nodeObjects)
	{
		ColliderType colliderType = object->ColliderBounding->Type;

		//if (colliderType == ColliderType::AABB)
		//{
			ColliderAABBptr objBoundingBox = dynamic_pointer_cast<ColliderAABB>(object->ColliderBounding);
			XMFLOAT3 dimensions = objBoundingBox->GetExtents() * 2.0f;
			DrawBox(dimensions, objBoundingBox->GetCenter(), view, projection, Collision::GetCollisionColor(objBoundingBox->CollisionKind));
		//}
		/*else
			if (colliderType == ColliderType::Sphere)
			{
				ColliderSpherePtr objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(object->ColliderBounding);
				float diameter = objBoundingSphere->GetRadius();
				DrawSphere(diameter, objBoundingSphere->GetCenter(), view, projection, Collision::GetCollisionColor(objBoundingSphere->CollisionKind));
			}*/
	}

	vector<shared_ptr<OctTree>> childrenNodes = octTree->ChildrenNodes;
	for each(OctTreePtr node in childrenNodes)
	{
		if (node != nullptr)
			DrawOctTree(node, view, projection, showTreeRegion);
	}
}


