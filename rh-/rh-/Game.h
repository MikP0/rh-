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

#include "PlayerSystem.h"
#include "PlayerComponent.h"
#include "Cooldown.h"

#include "HumanSystem.h"
#include "HumanComponent.h"

#include "WorldLoader.h"

//typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
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

	void RespawnRestart();

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

	//std::shared_ptr<DirectX::CommonStates> m_states;

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

	//Audio Entities
	std::shared_ptr<Entity> plotBackground;
	std::shared_ptr<Entity> gameBackground;	
	std::shared_ptr<Entity> fireBackground;
	std::shared_ptr<Entity> runningBackground;
	std::shared_ptr<Entity> deathBackground;
	std::shared_ptr<Entity> explodeBackground;
	std::shared_ptr<Entity> screamBackground;

	std::shared_ptr<Entity> playerFootstep;
	std::shared_ptr<Entity> playerNormalAttack;
	std::shared_ptr<Entity> playerSpinAttack;
	std::shared_ptr<Entity> playerBite;
	std::shared_ptr<Entity> playerDamage;

	std::shared_ptr<Entity> playerTeleport;
	std::shared_ptr<Entity> playerRipAttack;
	std::shared_ptr<Entity> playerSwap;
	std::shared_ptr<Entity> playerAoe;

	std::shared_ptr<Entity> enemyFootstep;
	std::shared_ptr<Entity> enemyAttack;
	std::shared_ptr<Entity> enemyDamage;
	std::shared_ptr<Entity> enemyDeath;
	std::shared_ptr<Entity> knighFootstep;


	std::shared_ptr<Entity> enemyEntity1;
	std::shared_ptr<Entity> enemyEntity2;
	std::shared_ptr<Entity> enemyEntity3;
	std::shared_ptr<Entity> enemyEntity4;
	std::shared_ptr<Entity> enemyEntity5;
	std::shared_ptr<Entity> enemyEntity6;
	std::shared_ptr<Entity> enemyEntity7;
	std::shared_ptr<Entity> enemyEntity8;
	std::shared_ptr<Entity> enemyEntity9;
	std::shared_ptr<Entity> enemyEntity10;
	std::shared_ptr<Entity> enemyEntity11;
	std::shared_ptr<Entity> enemyEntity12;
	std::shared_ptr<Entity> enemyEntity13;
	std::shared_ptr<Entity> enemyEntity14;
	std::shared_ptr<Entity> enemyEntity15;
	std::shared_ptr<Entity> BossEntity;

	// floor for shadows
	std::shared_ptr<Entity> myEntityFloor;

	// lights
	std::shared_ptr<Entity> pointLightEntity1;
	std::shared_ptr<Entity> spotLightEntity1;
	std::shared_ptr<Entity> directLightEntity1;

	// Collision boundings
	std::shared_ptr<PhysicsSystem> collisionSystem;
	std::shared_ptr<PhysicsComponent> colliderCup1;
	//std::shared_ptr<PhysicsComponent> colliderCup2;
	ColliderAABBptr colliderBoundingCup1;
	//ColliderSpherePtr colliderBoundingCup2;

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
	std::shared_ptr<AudioComponent> plotBackgroundAudio;
	std::shared_ptr<AudioComponent> gameBackgroundAudio;
	std::shared_ptr<AudioComponent> fireBackgroundAudio;
	std::shared_ptr<AudioComponent> runningBackgroundAudio;
	std::shared_ptr<AudioComponent> explodeBackgroundAudio;
	std::shared_ptr<AudioComponent> screamBackgroundAudio;

	std::shared_ptr<RenderableSystem> renderableSystem;


	//Enemy
	std::shared_ptr<EnemySystem> enemySystem;

	//Player
	std::shared_ptr<Entity> playerEntity;
	std::shared_ptr<PlayerSystem> playerSystem;
	std::shared_ptr<Cooldown> cooldown;
	vector<string> humanSkillsNames = { 
		"normalAttack", "strongAttack", "spinAttack", "biteAttack"
	};
	vector<string> vampireSkillsNames = {
		"teleport", "cleaveAttack", "swap","aoeAttack"
	};
	vector<bool> skillsBlockadeStates = {
		false, false, false, false,
		false, false, false, false,
	};
	vector<float> skillsTimeLimits = {
		0.0f, 3.0f, 5.0f, 10.0f
	};

	//human
	std::shared_ptr<HumanSystem> humanSystem;
	std::shared_ptr<Entity> humanEntity;

	//NavMesh
	std::shared_ptr<Terrain> terrain;

	std::shared_ptr<LightSystem> lightSystem;

	Keyboard::KeyboardStateTracker keyboardTracker;
	Mouse::ButtonStateTracker tracker;
	bool freeCameraLook = false;

	//UI
	shared_ptr<UI> Ui;

	float total_Time;
	float elapsed_Time;

	int menuIsOn = 0;
	bool debugDraw = false;
	bool debugDrawTreeRegions = true;

	//World
	std::shared_ptr<World> world;

	float brightness = 1.0f;

	//Vampire Mode
	bool vampireMode = false;


	//World Loader
	std::shared_ptr<WorldLoader> worldLoader;

	// Menu
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> startScreenTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> directXTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> wickedScreenTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mainMenuTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mainMenuCreditsTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> loadingScreenTexture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteBatch> m_ActualspriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPos;

	bool mainMenu = true;
	bool plotScreens = true;
	bool creditsScreen = false;

	int gameStage = 0;
	float startTimer = 0;

	void InitializeAll(ID3D11Device1 * device, ID3D11DeviceContext1 * context);
	bool initialized = false;
	void RestartAfterReplay();


	void UpdateMainMenu(float elapsedTime);
	void SkipPlot();
	void SkipStartScreen();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cattyTexture;	// cat Textrue to tests

	std::unique_ptr<AudioEngine> audEngine;
	std::unique_ptr<SoundEffect> menuBackground;
	std::unique_ptr<SoundEffectInstance> menuBackgroundAudio;
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot1Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot2Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot3Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot4Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot5Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot6Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot7Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot8Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot9Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> plot10Texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blackBackTexture;

	float ColorChanger = 0.0f;
	void ClearColorChanger();
	int remPlotStage = 0;

	int plotStage = 0;
	float plotTimer = 0;
	void ShowPlot(int stage);
	std::unique_ptr<CommonStates> states;


	//
	std::shared_ptr<Entity> swordEntity;

	bool humanMode = true;
	void SetHumanMode(bool check);
	bool skipper = false;
	bool AfterInitButBeforePlotFin = false;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap;

	bool isFire = false;
	bool isRunning = false;
	bool isExplode = false;
	bool isScream = false;

	//GameOver
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> gameOverTex;
	bool isGameOver = false;
	float countGameOver = 0.0;

	bool bossMode = false;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> endGameTex;
	bool endGame = false;
	float countEndGame = 0.0;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> thanksTex;
	bool thanksScreen = false;
	float countThanks = 0.0;
};