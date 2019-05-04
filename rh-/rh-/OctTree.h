#pragma once

#include <list>
#include <queue> 
#include <DirectXCollision.h>
#include "PhysicsComponent.h"

using namespace std;
using namespace DirectX;

class OctTree
{
public:
	OctTree();
	~OctTree();

private:
	BoundingBox _region;
	list<shared_ptr<PhysicsComponent>> _objects;
	static queue<shared_ptr<PhysicsComponent>> _pendingInsertion;
	shared_ptr<OctTree> _childNode[8];
	byte _activeNodes = 0;
	const int MIN_SIZE = 1;
	int _maxLifespan = 8;
	int m_curLife = -1;
	shared_ptr<OctTree> _parent;
	static bool _treeReady;
	static bool _treeBuilt;
};

