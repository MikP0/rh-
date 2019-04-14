//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "Entity.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "Camera.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

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

	void DrawCube(CXMMATRIX mWorld, FXMVECTOR color);
	void DrawAabb(const BoundingBox& box, FXMVECTOR color);
	void DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color);

private:

    void Update(DX::StepTimer const& timer);
    void Render();

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

	//ID3D11InputLayout* g_pBatchInputLayout = nullptr;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	//std::unique_ptr<DirectX::BasicEffect> g_BatchEffect;
	//std::unique_ptr<DirectX::PrimitiveBatch<VertexPositionColor>> g_Batch;

	// Model
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::Model> m_model2;

	// cup Object
	std::shared_ptr<Entity> myEntity1;
	std::shared_ptr<Entity> myEntity2;

	// Collision boundings
	std::shared_ptr<PhysicsComponent> colliderCup1;
	std::shared_ptr<PhysicsComponent> colliderCup2;
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
	std::unique_ptr<DirectX::GeometricPrimitive> m_boundingCup2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;
	
	// mouse settings
	float m_pitch;
	float m_yaw;
};