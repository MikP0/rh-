//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Audio.h"

#include "Transform.h"
#include "Entity.h"
#include "Camera.h"
#include "Raycast.h"
#include "Input.h"
#include "Collision.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "PhysicsComponent.h"
#include "RenderableSystem.h"

#include "PhysicsSystem.h"
#include "AudioSystem.h"

#include "ModelSkinned.h"

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
	void OnNewAudioDevice();

    // Properties
    void GetDefaultSize( int& width, int& height );

private:

    void Update(DX::StepTimer const& timer);
	void UpdateObjects(float elapsedTime);
    void Render();
	void RenderObjects(ID3D11DeviceContext1 *context);

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
	void InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context);

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

	//Managers
	std::shared_ptr<EntityManager> entityManager;
	std::shared_ptr<ComponentFactory> componentFactory;

	// Model
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::Model> m_model2;

	// cup Object
	std::shared_ptr<Entity> sceneWallEntity;
	std::shared_ptr<Entity> myEntity1;
	std::shared_ptr<Entity> myEntity2;
	std::shared_ptr<Entity> myEntity3;
	std::shared_ptr<Entity> myEntity4;

	// Collision boundings
	std::shared_ptr<PhysicsSystem> collisionSystem;
	std::shared_ptr<PhysicsComponent> colliderSceneWall;
	std::shared_ptr<PhysicsComponent> colliderCup1;
	std::shared_ptr<PhysicsComponent> colliderCup2;
	ColliderAABBptr colliderBoundingSceneWall;
	ColliderAABBptr colliderBoundingCup1;
	//ColliderSpherePtr colliderBoundingCup1;
	// ColliderAABBptr colliderBoundingCup2;
	ColliderSpherePtr colliderBoundingCup2;

	XMFLOAT3 initialBoundingEntity1Size;
	XMFLOAT3 initialBoundingEntity2Size;
	float initialBounding1Radius;
	float initialBounding2Radius;

	// camera
	Camera camera;

	// input
	std::map<availableKeys, actionList> actionKeysBindings = {
		{availableKeys::esc, actionList::closeWindow},
		{availableKeys::space, actionList::up},
		{availableKeys::leftControl, actionList::down},
		{availableKeys::a, actionList::left},
		{availableKeys::d, actionList::right},
		{availableKeys::w, actionList::forward},
		{availableKeys::s, actionList::backward},
		{availableKeys::lpm, actionList::anchorRotation},
		{availableKeys::upperArrow, actionList::moveFor},
		{availableKeys::lowerArrow, actionList::moveBac},
		{availableKeys::leftArrow, actionList::moveLeft},
		{availableKeys::rightArrow, actionList::moveRight},
		{availableKeys::one, actionList::special1},
		{availableKeys::two, actionList::special2},
		{availableKeys::three, actionList::special3},
		{availableKeys::four, actionList::special4},
		{availableKeys::z, actionList::playBackground},
		{availableKeys::x, actionList::playSound1},
	};
	std::shared_ptr<Entity> inputEntity;
	
	// primitives
	std::unique_ptr<DirectX::GeometricPrimitive> m_room;
	std::unique_ptr<DirectX::GeometricPrimitive> m_boundingEntity1;
	std::unique_ptr<DirectX::GeometricPrimitive> m_boundingEntity2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;

	// mouse settings
	float m_pitch;
	float m_yaw;

	// skinned model
	std::shared_ptr<ModelSkinned> mSkinModel;
	std::shared_ptr<Transform> mSkinModelTransform;

	// billboarding
	std::shared_ptr<DirectX::GeometricPrimitive> m_plane;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_planeTex;
	DirectX::SimpleMath::Matrix planeWorld;
	DirectX::SimpleMath::Vector3 planePos;

	std::shared_ptr<SceneManager> sceneManager;

	//Audio
	std::shared_ptr<AudioSystem> audioSystem;
	std::shared_ptr<AudioComponent> audioBackgroundSound;
	std::shared_ptr<AudioComponent> audioSound1;

	std::shared_ptr<RenderableSystem> renderableSystem;
};