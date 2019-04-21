#include "pch.h"
#include "RenderableComponent.h"


RenderableComponent::RenderableComponent()
{
}


RenderableComponent::~RenderableComponent()
{
}

ComponentType RenderableComponent::GetType(void)
{
	return ComponentType("Renderable");
}
