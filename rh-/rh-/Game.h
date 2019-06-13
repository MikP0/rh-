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
#include "PhysicsComponent.h"
#include "RenderableSystem.h"
#include "UI.h"

#include "PhysicsSystem.h"
#include "AudioSystem.h"

#include "ModelSkinned.h"
#include "ToonEffect.h"
#include "ToonFactory.h"
#include "LightSystem.h"
#include "LightComponent.h"

#include "NavMesh.h"
#include "Terrain.h"

#include "Coroutine.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"

#include "WorldLoader.h"

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
	void GetDefaultSize(int& width, int& height);

private:

	void Update(DX::StepTimer const& timer);
	void UpdateObjects(float elapsedTime);
	void UpdateAnimations(float elapsedTime);
	void UpdateCoroutines(float elapsedTime);
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

	std::shared_ptr<DirectX::CommonStates> m_states;

	//Shaders
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;

	// Model
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::Model> m_model2;

	// cup Object
	std::shared_ptr<Entity> myEntity1;
	std::shared_ptr<Entity> myEntity2;
	std::shared_ptr<Entity> myEntity3;
	std::shared_ptr<Entity> myEntity4;
	std::shared_ptr<Entity> myEntity5;
	std::shared_ptr<Entity> myEntity6;

	std::shared_ptr<Entity> playerEntity;

	std::shared_ptr<Entity> enemyEntity1;
	std::shared_ptr<Entity> enemyEntity2;

	// floor for shadows
	std::shared_ptr<Entity> myEntityFloor;

	// lights
	std::shared_ptr<Entity> pointLightEntity1;
	std::shared_ptr<Entity> spotLightEntity1;
	std::shared_ptr<Entity> directLightEntity1;

	// Collision boundings
	std::shared_ptr<PhysicsSystem> collisionSystem;
	std::shared_ptr<PhysicsComponent> colliderCup1;
	std::shared_ptr<PhysicsComponent> colliderCup2;
	ColliderAABBptr colliderBoundingCup1;
	ColliderSpherePtr colliderBoundingCup2;

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
		{availableKeys::c, actionList::freeCamera},
		{availableKeys::v, actionList::debugDrawAll},
		{availableKeys::b, actionList::debugDrawWithoutRegions}
	};

	// primitives
	//std::unique_ptr<DirectX::GeometricPrimitive> m_room;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;

	// mouse settings
	float m_pitch;
	float m_yaw;

	// billboarding
	std::shared_ptr<DirectX::GeometricPrimitive> m_plane;
	std::shared_ptr<DirectX::GeometricPrimitive> m_plane2;
	std::shared_ptr<DirectX::GeometricPrimitive> m_plane3;
	std::shared_ptr<DirectX::GeometricPrimitive> m_plane4;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_planeTex;
	DirectX::SimpleMath::Matrix planeWorld;
	DirectX::SimpleMath::Vector3 planePos;
	DirectX::SimpleMath::Matrix planeWorld2;
	DirectX::SimpleMath::Matrix planeWorld3;
	DirectX::SimpleMath::Matrix planeWorld4;

	//Audio
	std::shared_ptr<AudioSystem> audioSystem;
	std::shared_ptr<AudioComponent> audioBackgroundSound;
	std::shared_ptr<AudioComponent> audioSound1;

	std::shared_ptr<RenderableSystem> renderableSystem;


	//Enemy
	std::shared_ptr<EnemySystem> enemySystem;

	//NavMesh
	std::shared_ptr<NavMesh> navMesh;
	std::shared_ptr<Terrain> terrain;

	std::shared_ptr<LightSystem> lightSystem;

	Keyboard::KeyboardStateTracker keyboardTracker;
	Mouse::ButtonStateTracker tracker;
	bool freeCameraLook = false;

	//UI
	shared_ptr<UI> Ui;

	bool menuIsOn;
	bool debugDraw = false;
	bool debugDrawTreeRegions = true;

	//World
	std::shared_ptr<World> world;


	//Player
	bool isDancing = false;
	bool playerWalking = false;
	bool playerAttack = false;
	bool playerBite = false;
	bool enemyClicked = false;
	int attackType = 0;

	bool playerHealed = false;

	Coroutine playerAttackCorutine;
	Coroutine playerBiteCorutine;

	Coroutine enemyHittedCorutine;
	shared_ptr<Entity> hittedEnemy;

	Coroutine playerHittedCorutine;

	Coroutine playerHealedCorutine;

	float playerBiteDistance = 1.0f;
	float playerAttackDistance = 1.0f;
	shared_ptr<Entity> targetedEnemy;

	std::shared_ptr<float> playerHealth;
	float playerHealthOrigin = 100.0f;

	float brightness = 1.0f;

	//Vampire Mode
	bool vampireMode = false;

	int vampireAbility = 0;

	void StopEnemies();

	//World Loader
	std::shared_ptr<WorldLoader> worldLoader;
};