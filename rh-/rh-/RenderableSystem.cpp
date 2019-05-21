#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context) : System()
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_shared<ToonFactory>(_device);

	_componentsType = ComponentType("Renderable");
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::Iterate()
{

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_model != nullptr) {
			renderableComponent->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				renderableComponent->_camera->GetViewMatrix(),
				renderableComponent->_camera->GetProjectionMatrix()
			);
		}
		else 
		{
			renderableComponent->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				renderableComponent->_camera->GetViewMatrix(),
				renderableComponent->_camera->GetProjectionMatrix()
			);
		}
	}
}

void RenderableSystem::Initialize()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_modelPath.find(L".cmo") != std::wstring::npos) 
		{
			renderableComponent->_model =
				DirectX::Model::CreateFromCMO(_device, renderableComponent->_modelPath.c_str(), *_fxFactory);
		}
		else 
		{
			renderableComponent->_modelSkinned =
				std::make_unique<ModelSkinned>(_device, converter.to_bytes(renderableComponent->_modelPath.c_str()), _context);

		}
	}
}