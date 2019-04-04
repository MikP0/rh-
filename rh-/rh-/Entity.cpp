//
// Enity.cpp
//

#include "Entity.h"

Entity::Entity(int id, std::string name, DirectX::SimpleMath::Matrix position, DirectX::Model *model)
{
	ID = id;
	Name = name;
	Position = position;
	Model = std::unique_ptr<DirectX::Model>(model);
}

Entity::Entity()
{

}