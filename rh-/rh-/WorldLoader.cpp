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

	tx::XMLNode * root = xmlLevel.FirstChild();
	if (root != nullptr)
	{
		tx::XMLElement* eEntities = root->FirstChildElement("Entities");
		if (eEntities != nullptr)
		{
			tx::XMLElement* eEntity = eEntities->FirstChildElement("Entity");
			while (eEntity)
			{
				auto oEntity = _world->CreateEntity(eEntity->FirstAttribute()->Value());

				tx::XMLElement* eRotation = eEntity->FirstChildElement("Rotation");
				if (eRotation != nullptr)
				{
					DirectX::SimpleMath::Vector3 aAxis;
					float aAngle;

					tx::XMLElement* eAxis = eRotation->FirstChildElement("Axis");
					if (eAxis != nullptr)
					{
						tx::XMLElement* eXRotation = eAxis->FirstChildElement("x");
						if (eXRotation != nullptr)
						{
							aAxis.x = atof(eXRotation->GetText());
						}

						tx::XMLElement* eYRotation = eAxis->FirstChildElement("y");
						if (eYRotation != nullptr)
						{
							aAxis.y = atof(eYRotation->GetText());
						}

						tx::XMLElement* eZRotation = eAxis->FirstChildElement("z");
						if (eZRotation != nullptr)
						{
							aAxis.z = atof(eZRotation->GetText());
						}
					}

					tx::XMLElement* eAngle = eRotation->FirstChildElement("Angle");
					if (eAngle != nullptr)
					{
						aAngle = DirectX::XMConvertToRadians(atof(eAngle->GetText()));
					}
					oEntity->GetTransform()->Rotate(DirectX::SimpleMath::Vector3(0,1,0), DirectX::XMConvertToRadians(180));
					if(aAngle != 0)
						oEntity->GetTransform()->Rotate(aAxis, aAngle);
				}

				tx::XMLElement* ePosition = eEntity->FirstChildElement("Position");
				if(ePosition != nullptr)
				{
					tx::XMLElement* eXCoord = ePosition->FirstChildElement("x");
					if (eXCoord != nullptr)
					{
						oEntity->GetTransform()->SetX(atof(eXCoord->GetText()));
					}

					tx::XMLElement* eYCoord = ePosition->FirstChildElement("y");
					if (eYCoord != nullptr)
					{
						oEntity->GetTransform()->SetY(atof(eYCoord->GetText()));
					}

					tx::XMLElement* eZCoord = ePosition->FirstChildElement("z");
					if (eZCoord != nullptr)
					{
						oEntity->GetTransform()->SetZ(atof(eZCoord->GetText()));
					}
				}

				tx::XMLElement* eScale = eEntity->FirstChildElement("Scale");
				if (eScale != nullptr)
				{
					DirectX::SimpleMath::Vector3 aScale;
					tx::XMLElement* eXScale = eScale->FirstChildElement("x");
					if (eXScale != nullptr)
					{
						aScale.x = atof(eXScale->GetText());
					}

					tx::XMLElement* eYScale = eScale->FirstChildElement("y");
					if (eYScale != nullptr)
					{
						aScale.y = atof(eYScale->GetText());
					}

					tx::XMLElement* eZScale = eScale->FirstChildElement("z");
					if (eZScale != nullptr)
					{
						aScale.z = atof(eZScale->GetText());
					}
					oEntity->GetTransform()->SetScale(aScale);
				}

				tx::XMLElement* eComponents = eEntity->FirstChildElement("Components");
				if (eComponents != nullptr)
				{
					tx::XMLElement* eRenderableComponent = eComponents->FirstChildElement("RenderableComponent");
					if (eRenderableComponent != nullptr)
					{
						std::wstring aPath;
						bool aShadow = false;
						bool castShadow = true;
						bool reflectThat = false;
						bool aIgnoreShadow = false;

						tx::XMLElement* eRenderableComponentPath = eRenderableComponent->FirstChildElement("Path");
						if (eRenderableComponentPath != nullptr)
						{
							std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
							aPath = converter.from_bytes(eRenderableComponentPath->GetText());
						}

						tx::XMLElement* eRenderableComponentShadow = eRenderableComponent->FirstChildElement("Shadow");
						if (eRenderableComponentShadow != nullptr)
						{
							aShadow = atoi(eRenderableComponentShadow->GetText());	
						}
						else if (std::string(oEntity->GetName()).find("Floor") != std::string::npos)
							aShadow = true;

						if (std::string(oEntity->GetName()).find("Floor") != std::string::npos)
							castShadow = false;
						else if (std::string(oEntity->GetName()).find("Ceiling") != std::string::npos)
							castShadow = false;
						else if (std::string(oEntity->GetName()).find("Wall") != std::string::npos)
							castShadow = false;
						else if (std::string(oEntity->GetName()).find("lamp") != std::string::npos)
							castShadow = false;
						else if (std::string(oEntity->GetName()).find("chains") != std::string::npos)
							castShadow = false;
						else if (std::string(oEntity->GetName()).find("Torch") != std::string::npos)
							castShadow = false;

						/*if (std::string(oEntity->GetName()).find("Coffin_Open") != std::string::npos)
							reflectThat = true;*/

						if (std::string(oEntity->GetName()).find("Ceiling") != std::string::npos)
							aIgnoreShadow = true;

						oEntity->AddComponent<RenderableComponent>(aPath, _camera, aShadow, castShadow, reflectThat, aIgnoreShadow);
					}

					tx::XMLElement* eLightComponent = eComponents->FirstChildElement("LightComponent");
					if (eLightComponent != nullptr)
					{
						if (eLightComponent->FirstAttribute()->Value() == std::string("DirectLight"))
						{
							DirectX::XMFLOAT4 aColor;
							DirectX::XMFLOAT3 aDirection;

							tx::XMLElement* eLightComponentColor = eLightComponent->FirstChildElement("Color");
							if (eLightComponentColor != nullptr)
							{
								tx::XMLElement* eLightComponentColor_x = eLightComponentColor->FirstChildElement("x");
								if (eLightComponentColor_x != nullptr)
								{
									aColor.x = atof(eLightComponentColor_x->GetText());
								}
								tx::XMLElement* eLightComponentColor_y = eLightComponentColor->FirstChildElement("y");
								if (eLightComponentColor_y != nullptr)
								{
									aColor.y = atof(eLightComponentColor_y->GetText());
								}
								tx::XMLElement* eLightComponentColor_z = eLightComponentColor->FirstChildElement("z");
								if (eLightComponentColor_z != nullptr)
								{
									aColor.z = atof(eLightComponentColor_z->GetText());
								}
								tx::XMLElement* eLightComponentColor_w = eLightComponentColor->FirstChildElement("w");
								if (eLightComponentColor_w != nullptr)
								{
									aColor.w = atof(eLightComponentColor_w->GetText());
								}

							}
							tx::XMLElement* eLightComponentDirection = eLightComponent->FirstChildElement("Direction");
							if (eLightComponentDirection != nullptr)
							{
								tx::XMLElement* eLightComponentDirection_x = eLightComponentDirection->FirstChildElement("x");
								if (eLightComponentDirection_x != nullptr)
								{
									aDirection.x = atof(eLightComponentDirection_x->GetText());
								}
								tx::XMLElement* eLightComponentDirection_y = eLightComponentDirection->FirstChildElement("y");
								if (eLightComponentDirection_y != nullptr)
								{
									aDirection.y = atof(eLightComponentDirection_y->GetText());
								}
								tx::XMLElement* eLightComponentDirection_z = eLightComponentDirection->FirstChildElement("z");
								if (eLightComponentDirection_z != nullptr)
								{
									aDirection.z = atof(eLightComponentDirection_z->GetText());
								}
							}
							oEntity->AddComponent<LightComponent>(aColor, aDirection);
						}
						else if (eLightComponent->FirstAttribute()->Value() == std::string("SpotLight"))
						{
							DirectX::XMFLOAT4 aColor;
							DirectX::XMFLOAT3 aDirection;
							float aOuterAngle;
							DirectX::XMFLOAT3 aPosition;
							float aInnerAngle;
							float aRadius;
							bool aUpdatable;

							tx::XMLElement* eLightComponentColor = eLightComponent->FirstChildElement("Color");
							if (eLightComponentColor != nullptr)
							{
								tx::XMLElement* eLightComponentColor_x = eLightComponentColor->FirstChildElement("x");
								if (eLightComponentColor_x != nullptr)
								{
									aColor.x = atof(eLightComponentColor_x->GetText());
								}
								tx::XMLElement* eLightComponentColor_y = eLightComponentColor->FirstChildElement("y");
								if (eLightComponentColor_y != nullptr)
								{
									aColor.y = atof(eLightComponentColor_y->GetText());
								}
								tx::XMLElement* eLightComponentColor_z = eLightComponentColor->FirstChildElement("z");
								if (eLightComponentColor_z != nullptr)
								{
									aColor.z = atof(eLightComponentColor_z->GetText());
								}
								tx::XMLElement* eLightComponentColor_w = eLightComponentColor->FirstChildElement("w");
								if (eLightComponentColor_w != nullptr)
								{
									aColor.w = atof(eLightComponentColor_w->GetText());
								}
							}
							tx::XMLElement* eLightComponentDirection = eLightComponent->FirstChildElement("Direction");
							if (eLightComponentDirection != nullptr)
							{
								tx::XMLElement* eLightComponentDirection_x = eLightComponentDirection->FirstChildElement("x");
								if (eLightComponentDirection_x != nullptr)
								{
									aDirection.x = atof(eLightComponentDirection_x->GetText());
								}
								tx::XMLElement* eLightComponentDirection_y = eLightComponentDirection->FirstChildElement("y");
								if (eLightComponentDirection_y != nullptr)
								{
									aDirection.y = atof(eLightComponentDirection_y->GetText());
								}
								tx::XMLElement* eLightComponentDirection_z = eLightComponentDirection->FirstChildElement("z");
								if (eLightComponentDirection_z != nullptr)
								{
									aDirection.z = atof(eLightComponentDirection_z->GetText());
								}
							}
							tx::XMLElement* eLightComponentOuterAngle = eLightComponent->FirstChildElement("OuterAngle");
							if (eLightComponentOuterAngle != nullptr)
							{
								aOuterAngle = atof(eLightComponentOuterAngle->GetText());
							}
							tx::XMLElement* eLightComponentPosition = eLightComponent->FirstChildElement("Position");
							if (eLightComponentPosition != nullptr)
							{
								tx::XMLElement* eLightComponentPosition_x = eLightComponent->FirstChildElement("x");
								if (eLightComponentPosition_x != nullptr)
								{
									aPosition.x = atof(eLightComponentPosition_x->GetText());
								}
								tx::XMLElement* eLightComponentPosition_y = eLightComponent->FirstChildElement("y");
								if (eLightComponentPosition_y != nullptr)
								{
									aPosition.y = atof(eLightComponentPosition_y->GetText());
								}
								tx::XMLElement* eLightComponentPosition_z = eLightComponent->FirstChildElement("z");
								if (eLightComponentPosition_z != nullptr)
								{
									aPosition.z = atof(eLightComponentPosition_z->GetText());
								}
							}
							tx::XMLElement* eLightComponentInnerAngle = eLightComponent->FirstChildElement("InnerAngle");
							if (eLightComponentInnerAngle != nullptr)
							{
								aInnerAngle = atof(eLightComponentInnerAngle->GetText());
							}
							tx::XMLElement* eLightComponentRadius = eLightComponent->FirstChildElement("Radius");
							if (eLightComponentRadius != nullptr)
							{
								aRadius = atof(eLightComponentRadius->GetText());
							}
							tx::XMLElement* eLightComponentUpdatable = eLightComponent->FirstChildElement("Updatable");
							if (eLightComponentUpdatable != nullptr)
							{
								aUpdatable = atoi(eLightComponentUpdatable->GetText());
							}
							oEntity->AddComponent<LightComponent>(aColor, aDirection, aOuterAngle, aPosition, aInnerAngle, aRadius, aUpdatable);
						}
						else if (eLightComponent->FirstAttribute()->Value() == std::string("PointLight"))
						{
							DirectX::XMFLOAT4 aColor;
							DirectX::SimpleMath::Vector3 aPosition2;
							float aRadius;
							bool aUpdatable;

							tx::XMLElement* eLightComponentColor = eLightComponent->FirstChildElement("Color");
							if (eLightComponentColor != nullptr)
							{
								tx::XMLElement* eLightComponentColor_x = eLightComponentColor->FirstChildElement("x");
								if (eLightComponentColor_x != nullptr)
								{
									aColor.x = atof(eLightComponentColor_x->GetText());
								}
								tx::XMLElement* eLightComponentColor_y = eLightComponentColor->FirstChildElement("y");
								if (eLightComponentColor_y != nullptr)
								{
									aColor.y = atof(eLightComponentColor_y->GetText());
								}
								tx::XMLElement* eLightComponentColor_z = eLightComponentColor->FirstChildElement("z");
								if (eLightComponentColor_z != nullptr)
								{
									aColor.z = atof(eLightComponentColor_z->GetText());
								}
								tx::XMLElement* eLightComponentColor_w = eLightComponentColor->FirstChildElement("w");
								if (eLightComponentColor_w != nullptr)
								{
									aColor.w = atof(eLightComponentColor_w->GetText());
								}
							}
							tx::XMLElement* eLightComponentPosition = eLightComponent->FirstChildElement("Position");
							if (eLightComponentPosition != nullptr)
							{
								tx::XMLElement* eLightComponentPosition_x = eLightComponentPosition->FirstChildElement("x");
								if (eLightComponentPosition_x != nullptr)
								{
									aPosition2.x = atof(eLightComponentPosition_x->GetText());
								}
								tx::XMLElement* eLightComponentPosition_y = eLightComponentPosition->FirstChildElement("y");
								if (eLightComponentPosition_y != nullptr)
								{
									aPosition2.y = atof(eLightComponentPosition_y->GetText());
								}
								tx::XMLElement* eLightComponentPosition_z = eLightComponentPosition->FirstChildElement("z");
								if (eLightComponentPosition_z != nullptr)
								{
									aPosition2.z = atof(eLightComponentPosition_z->GetText());
								}
							}
							tx::XMLElement* eLightComponentRadius = eLightComponent->FirstChildElement("Radius");
							if (eLightComponentRadius != nullptr)
							{
								aRadius = atof(eLightComponentRadius->GetText());
							}
							tx::XMLElement* eLightComponentUpdatable = eLightComponent->FirstChildElement("Updatable");
							if (eLightComponentUpdatable != nullptr)
							{
								aUpdatable = atoi(eLightComponentUpdatable->GetText());
							}
							oEntity->AddComponent<LightComponent>(aColor, oEntity->GetTransform()->GetPosition() + aPosition2, aRadius, aUpdatable);
						}
					}

					tx::XMLElement* eAudioComponent = eComponents->FirstChildElement("AudioComponent");
					if (eAudioComponent != nullptr) 
					{
						tx::XMLElement* eAudioComponentPath = eAudioComponent->FirstChildElement("Path");
						if(eAudioComponentPath != nullptr)
						{
							oEntity->AddComponent<AudioComponent>(eAudioComponentPath->GetText());
						}
					}

					tx::XMLElement* ePhysicsComponent = eComponents->FirstChildElement("PhysicsComponent");
					if(ePhysicsComponent != nullptr)
					{
						DirectX::SimpleMath::Vector3 aPositionOffset = DirectX::SimpleMath::Vector3::Zero;
						DirectX::XMFLOAT3 aExtents;
						bool aIsTriggered;

						tx::XMLElement* ePhysicsComponentPositionOffset = ePhysicsComponent->FirstChildElement("PositionOffset");
						if(ePhysicsComponentPositionOffset != nullptr)
						{
							tx::XMLElement* ePhysicsComponentPositionOffset_x = ePhysicsComponentPositionOffset->FirstChildElement("x");
							if(ePhysicsComponentPositionOffset_x != nullptr)
							{
								aPositionOffset.x = atof(ePhysicsComponentPositionOffset_x->GetText());
							}
							tx::XMLElement* ePhysicsComponentPositionOffset_y = ePhysicsComponentPositionOffset->FirstChildElement("y");
							if (ePhysicsComponentPositionOffset_y != nullptr)
							{
								aPositionOffset.y = atof(ePhysicsComponentPositionOffset_y->GetText());
							}
							tx::XMLElement* ePhysicsComponentPositionOffset_z = ePhysicsComponentPositionOffset->FirstChildElement("z");
							if (ePhysicsComponentPositionOffset_z != nullptr)
							{
								aPositionOffset.z = atof(ePhysicsComponentPositionOffset_z->GetText());
							}
						}

						tx::XMLElement* ePhysicsComponentExtents = ePhysicsComponent->FirstChildElement("Extents");
						if(ePhysicsComponentExtents != nullptr)
						{
							tx::XMLElement* ePhysicsComponentExtents_x = ePhysicsComponentExtents->FirstChildElement("x");
							if(ePhysicsComponentExtents_x != nullptr)
							{
								aExtents.x = atof(ePhysicsComponentExtents_x->GetText());
							}
							tx::XMLElement* ePhysicsComponentExtents_y = ePhysicsComponentExtents->FirstChildElement("y");
							if (ePhysicsComponentExtents_y != nullptr)
							{
								aExtents.y = atof(ePhysicsComponentExtents_y->GetText());
							}
							tx::XMLElement* ePhysicsComponentExtents_z = ePhysicsComponentExtents->FirstChildElement("z");
							if (ePhysicsComponentExtents_z != nullptr)
							{
								aExtents.z = atof(ePhysicsComponentExtents_z->GetText());
							}
						}

						tx::XMLElement* ePhysicsComponentIsTriggered = ePhysicsComponent->FirstChildElement("IsTriggered");
						if(ePhysicsComponentIsTriggered != nullptr)
						{
							aIsTriggered = atoi(ePhysicsComponentIsTriggered->GetText());
						}
						oEntity->AddComponent<PhysicsComponent>(aPositionOffset, aExtents, aIsTriggered);
					}
				}
				eEntity = eEntity->NextSiblingElement("Entity");
			}
		}
	}
}
