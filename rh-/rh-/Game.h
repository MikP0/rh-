//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include "Transform.h"
#include "Entity.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "Camera.h"

typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);
	void InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
	void UpdateObjects();
    void Render();
	void RenderObjects(ID3D11DeviceContext1 *context);

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	// Matrix
	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;

	// Model
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::Model> m_model2;

	// cup Object
	std::shared_ptr<Entity> myEntity1;
	std::shared_ptr<Entity> myEntity2;

	// Collision boundings
	std::shared_ptr<PhysicsComponent> colliderCup1;
	std::shared_ptr<PhysicsComponent> colliderCup2;
	ColliderAABBptr colliderBoundingCup1;
	//ColliderSpherePtr colliderBoundingCup1;
	// ColliderAABBptr colliderBoundingCup2;
	ColliderSpherePtr colliderBoundingCup2;
	std::shared_ptr<PhysicsSystem> collisionSystem;
	
	// camera
	Camera camera;

	// input
	/*std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;*/
	std::map<availableKeys, actionList> actionKeysBindings = {
		{esc, closeWindow},
		{space, up},
		{leftControl, down},
		{a, left},
		{d, right},
		{w, forward},
		{s, backward},
		{lpm, anchorRotation}
	};
	std::shared_ptr<Entity> inputEntity;
	std::shared_ptr<InputComponent> inputComponent;
	std::shared_ptr<InputSystem> inputSystem;

	// room
	std::unique_ptr<DirectX::GeometricPrimitive> m_room;
	std::unique_ptr<DirectX::GeometricPrimitive> m_boundingEntity1;
	std::unique_ptr<DirectX::GeometricPrimitive> m_boundingEntity2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;
	
	XMFLOAT3 initialBoundingEntity1Size;
	XMFLOAT3 initialBoundingEntity2Size;
	float initialBounding1Radius;
	float initialBounding2Radius;

	// mouse settings
	float m_pitch;
	float m_yaw;
};