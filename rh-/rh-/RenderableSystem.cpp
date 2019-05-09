#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(std::shared_ptr<EntityManager> entityManager, ID3D11Device1* device, ID3D11DeviceContext1* context) : System(entityManager)
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_unique<ToonFactory>(_device);

	_componentsType = ComponentType("Renderable");
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::Iterate()
{

	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model != nullptr) {
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
			);
		}
		else 
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
			);
		}
	}
}

void RenderableSystem::Initialize()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.find(L".cmo") != std::wstring::npos) 
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model =
				DirectX::Model::CreateFromCMO(_device,
					std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.c_str(),
					*_fxFactory);
		}
		else 
		{
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned =
					std::make_unique<ModelSkinned>(_device,
						converter.to_bytes(std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.c_str()),
						_context);

		}
	}
}
