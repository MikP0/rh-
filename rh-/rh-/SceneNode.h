#pragma once

#include "pch.h"
#include <string>

class SceneNode : public RTTI
{
	RTTI_DECLARATIONS(SceneNode, RTTI)

public:
	const std::string& Name() const;
	SceneNode* Parent();
	std::vector<SceneNode*>& Children();
	const DirectX::XMFLOAT4X4& Transform() const;
	DirectX::XMMATRIX TransformMatrix() const;

	void SetParent(SceneNode* parent);

	void SetTransform(DirectX::XMFLOAT4X4& transform);
	void SetTransform(DirectX::CXMMATRIX transform);

	SceneNode(const std::string& name);
	SceneNode(const std::string& name, const DirectX::XMFLOAT4X4& transform);


protected:
	std::string mName;
	SceneNode* mParent;
	std::vector<SceneNode*> mChildren;
	DirectX::XMFLOAT4X4 mTransform;

private:
	SceneNode();
	SceneNode(const SceneNode& rhs);
	SceneNode& operator=(const SceneNode& rhs);
};

