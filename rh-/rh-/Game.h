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
#include "ToonEffect.h"
#include "ToonFactory.h"
#include "LightSystem.h"
#include "LightComponent.h"

#include "NavMesh.h"
#include "Terrain.h"

#include "ShadowMap.h"



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




	//
	void BuildShadowTransform(ID3D11DeviceContext1 *context);



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

	//Shaders
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

	// lights
	std::shared_ptr<Entity> pointLightEntity1;
	std::shared_ptr<Entity> pointLightEntity2;
	std::shared_ptr<Entity> pointLightEntity3;
	std::shared_ptr<Entity> spotLightEntity1;
	std::shared_ptr<Entity> directLightEntity1;

	// Collision boundings
	std::shared_ptr<PhysicsSystem> collisionSystem;
	std::shared_ptr<PhysicsComponent> colliderSceneWall;
	std::shared_ptr<PhysicsComponent> colliderCup1;
	std::shared_ptr<PhysicsComponent> colliderCup2;
	ColliderAABBptr colliderBoundingSceneWall;
	ColliderAABBptr colliderBoundingCup1;
	ColliderSpherePtr colliderBoundingCup2;

	XMFLOAT3 initialBoundingEntity1Size;
	XMFLOAT3 initialBoundingEntity2Size;
	float initialBounding1Radius;
	float initialBounding2Radius;

	// camera
	Camera camera;


	float bardzowaznazmiennaalledousuniecia = 0;
	bool clicked = false;


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
	std::shared_ptr<Entity> inputEntity;

	// primitives
	//std::unique_ptr<DirectX::GeometricPrimitive> m_room;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_roomTex;

	// mouse settings
	float m_pitch;
	float m_yaw;

	// skinned model
	std::shared_ptr<ModelSkinned> mSkinModel;
	std::shared_ptr<Transform> mSkinModelTransform;

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

	std::shared_ptr<SceneManager> sceneManager;

	//Audio
	std::shared_ptr<AudioSystem> audioSystem;
	std::shared_ptr<AudioComponent> audioBackgroundSound;
	std::shared_ptr<AudioComponent> audioSound1;

	std::shared_ptr<RenderableSystem> renderableSystem;

	//NavMesh
	std::shared_ptr<NavMesh> navMesh;
	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<LightSystem> lightSystem;

	Mouse::ButtonStateTracker tracker;
	bool freeCameraLook = false;

	//UI Sprites
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarHeroTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarHealthTex;
	DirectX::SimpleMath::Vector2 healthBarPos;
	DirectX::SimpleMath::Vector2 healthBarHeroPos;
	DirectX::SimpleMath::Vector2 healthBarHealthPos;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fpsBarTex;
	DirectX::SimpleMath::Vector2 fpsBarPos;

	std::shared_ptr<DirectX::SpriteBatch> uiSpriteBatch;

	std::shared_ptr<DirectX::SpriteFont> fpsFont;
	DirectX::SimpleMath::Vector2 fpsFontPos;
	std::wstring fpsFontText;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> menuTex;
	DirectX::SimpleMath::Vector2 menuPos;

	bool menuIsOn;

	//Debug Tree
	bool debugDraw = false;
	bool debugDrawTreeRegions = true;

	//Shadows
	std::shared_ptr<Entity> myEntityWall;

	std::shared_ptr<ShadowMap> m_shadowMap;
	void XM_CALLCONV DrawSenceToShadowMap(FXMMATRIX view, FXMMATRIX projection);
	


	std::shared_ptr<ShadowFactory> m_shadowFactory;
	std::shared_ptr<ToonFactory> m_toonFactory;

	bool isFirstCup = false;
	bool isSecondCup = false;
	bool isThirdCup = false;

	bool isFordCup = false;

	std::shared_ptr<Entity> entityCup1;
	std::shared_ptr<Entity> entityCup2;
	std::shared_ptr<Entity> entityCup3;


	std::shared_ptr<Entity> testShadowEntity;
	std::shared_ptr<DirectX::Model> testShadowModel;

	XMMATRIX lightView;
	XMMATRIX lightProj;
	XMMATRIX lightShadowTransform;


	
};