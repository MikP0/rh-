#include "pch.h"
#include "WorldLoader.h"



#include <codecvt>

namespace tx = tinyxml2;

WorldLoader::WorldLoader(std::shared_ptr<World> world, Camera* camera)
{
	_world = world;
	_camera = camera;
}

WorldLoader::~WorldLoader()
{
}

void WorldLoader::LoadWorldFromXML(std::string filename)
{
	tx::XMLDocument xmlLevel;

	tx::XMLError eResult = xmlLevel.LoadFile(filename.c_str());
	if (eResult != tx::XML_SUCCESS) return;

	tx::XMLNode* root = xmlLevel.FirstChild();
	if (root != nullptr) 
	{
		tx::XMLElement* eEntities = root->FirstChildElement("Entities");
		if (eEntities != nullptr) 
		{
			tx::XMLElement* eEntity = eEntities->FirstChildElement("Entity");
			while(eEntity)
			{
				auto oEntity = _world->CreateEntity(eEntity->FirstAttribute()->Value());
				
				tx::XMLElement* eXCoord = eEntity->FirstChildElement("x");
				if (eXCoord != nullptr) 
				{
					oEntity->GetTransform()->SetX(atof(eXCoord->GetText()));
				}

				tx::XMLElement* eYCoord = eEntity->FirstChildElement("y");
				if(eYCoord != nullptr)
				{
					oEntity->GetTransform()->SetY(atof(eYCoord->GetText()));
				}

				tx::XMLElement* eZCoord = eEntity->FirstChildElement("z");
				if(eZCoord != nullptr)
				{
					oEntity->GetTransform()->SetZ(atof(eZCoord->GetText()));
				}

				tx::XMLElement* eComponent = eEntity->FirstChildElement("Component");
				while(eComponent)
				{
					tx::XMLElement* eRenderableComponent = eComponent->FirstChildElement("RenderableComponent");
					if (eRenderableComponent != nullptr) 
					{
						tx::XMLElement* eRenderableComponentPath = eRenderableComponent->FirstChildElement("Path");
						if(eRenderableComponentPath != nullptr)
						{
							std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
							oEntity->AddComponent<RenderableComponent>(converter.from_bytes(eRenderableComponentPath->GetText()), _camera);
						}
					}
					eComponent = eComponent->NextSiblingElement("Component");
				}
				eEntity = eEntity->NextSiblingElement("Entity");
			}
		}
	}
	

}
