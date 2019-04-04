//
// Entity.h
//

#pragma once
#include <string>
#include "pch.h"

class Entity
{
public:
	Entity();
	Entity(int id, std::string name, DirectX::SimpleMath::Matrix position, DirectX::Model *model);


	int ID;
	//char Tag;
	std::string Name;

	DirectX::SimpleMath::Matrix Position;
	std::unique_ptr<DirectX::Model> Model;
private:

	
};