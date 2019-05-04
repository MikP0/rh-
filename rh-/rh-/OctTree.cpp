#include "pch.h"
#include "OctTree.h"

bool OctTree::_treeReady = false;
bool OctTree::_treeBuilt = false;

OctTree::OctTree()
{
	for (int i = 0; i < 8; i++)
		_childNode[i] = make_shared<OctTree>();
}


OctTree::~OctTree()
{
}
