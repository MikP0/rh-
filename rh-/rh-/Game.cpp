//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "System.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"
#include "AudioComponent.h"

#include <thread>
#include <chrono>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace
{
	const XMVECTORF32 ROOM_BOUNDS = { 1.f, 0.f, 1.f, 0.f }; //REMOVE
	const float COLLISION_SCENE_RANGE = 15.0f; // Octtree construcor
	const Vector3 SCENE_CENTER = Vector3::Zero; //Octtree constructor
	const float ROTATION_GAIN = 0.008f; // REMOVE
	const float MOVEMENT_GAIN = 0.07f; // REMOVE

	const XMVECTORF32 PLANE_BOUNDS = { 1.5f, 1.5f, 1.5f, 0.f }; //REMOVE
}

Game::Game() noexcept(false) : m_pitch(0.f), m_yaw(0.f)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	Input::SetWindowForMouse(window);
	Input::AvailableKeysActionsBinding = actionKeysBindings;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());
	float time = float(timer.GetTotalSeconds());
	float fps = timer.GetFramesPerSecond();


	// TODO: Add your game logic here.

	Vector3 move = Vector3::Zero;

	//Mouse
	auto mouse = Input::GetMouseState();
	if (freeCamera)
		Input::SetMouseMode(mouse.middleButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	//Pressed Keys
	std::vector<actionList> pushedKeysActions = Input::GetActions();
	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter) // REMOVE Action list
	{
		if (*iter == closeWindow)
		{
			//ExitGame();
			menuIsOn = true;
		}

		if (!menuIsOn)
		{
			if (freeCamera) {
				if (*iter == up)
					move.y += 1.f;

				if (*iter == down)
					move.y -= 1.f;

				if (*iter == actionList::left)
					move.x += 1.f;

				if (*iter == actionList::right)
					move.x -= 1.f;

				if (*iter == actionList::forward)
					move.z += 1.f;

				if (*iter == actionList::backward)
					move.z -= 1.f;
			}

			if (*iter == special1)
			{
				for (auto component : world->GetComponents<RenderableComponent>())
				{
					if (strcmp(component->GetParent()->GetName().c_str(),
						"Player") == 0)
					{
						component->_modelSkinned->GetAnimatorPlayer()->StartClip("HipHop"); // -> Player Entity -> Player Component 
						component->_modelSkinned->SetInMove(true);
						component->_modelSkinned->GetAnimatorPlayer()->SetDirection(true);
					}
				}
			}

			if (*iter == special2)
			{
				for (auto component : world->GetComponents<RenderableComponent>())
				{
					if (strcmp(component->GetParent()->GetName().c_str(),
						"Player") == 0)
					{
						component->_modelSkinned->GetAnimatorPlayer()->StartClip("Dance"); // -> Player Entity -> Player Component
						component->_modelSkinned->SetInMove(true);
						component->_modelSkinned->GetAnimatorPlayer()->SetDirection(true);
					}
				}
			}

			if (*iter == playBackground)
			{
				audioBackgroundSound->Mute = false;
			}

			if (*iter == playSound1)
			{
				audioSound1->Mute = false;

				if ((healthBarHealthPos.x <= 135.0f) && (healthBarHealthPos.x >= -150.0f))
					healthBarHealthPos.x -= 5.f;
			}

			if (*iter == freeCamera) {
				freeCameraLook = !freeCameraLook;
			}

			if (*iter == debugDrawAll)
			{
				debugDraw = !debugDraw;
			}

			if (*iter == debugDrawWithoutRegions)
			{
				debugDrawTreeRegions = !debugDrawTreeRegions;
			}
		}
	}

	if (!menuIsOn)
	{

		if (!navMesh->isMoving)
		{
			for (auto component : world->GetComponents<RenderableComponent>())
			{
				if (strcmp(component->GetParent()->GetName().c_str(),
					"Player") == 0)
				{
					component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
					component->_modelSkinned->SetInMove(true);
				}
			}
		}

		else 
		{
			for (auto component : world->GetComponents<RenderableComponent>())
			{
				if (strcmp(component->GetParent()->GetName().c_str(),
					"Player") == 0)
				{
					component->_modelSkinned->GetAnimatorPlayer()->StartClip("Walk");
					component->_modelSkinned->SetInMove(true);
					component->_modelSkinned->GetAnimatorPlayer()->SetDirection(true);
				}
			}
		}
	}

	//Audio

	if (!menuIsOn)
	{

		//Camera Movement
		if (freeCameraLook) {
			if (mouse.positionMode == Mouse::MODE_RELATIVE)
			{
				Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * ROTATION_GAIN;

				m_pitch -= delta.y;
				m_yaw -= delta.x;

				// limit pitch to straight up or straight down with a little fudge-factor to avoid gimbal lock
				float limit = XM_PI / 2.0f - 0.01f;
				m_pitch = std::max(-limit, m_pitch);
				m_pitch = std::min(+limit, m_pitch);

				// keep longitude in sane range by wrapping
				if (m_yaw > XM_PI)
				{
					m_yaw -= XM_PI * 2.0f;
				}
				else if (m_yaw < -XM_PI)
				{
					m_yaw += XM_PI * 2.0f;
				}
			}

			move = Vector3::Transform(move, Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f));
			move *= MOVEMENT_GAIN;
			Vector3 tempCamera = camera.GetPositionVector();
			tempCamera += move;
			camera.SetPosition(tempCamera);
			camera.SetPitch(m_pitch);
			camera.SetYaw(m_yaw);
		}

		else {
			camera.SetPosition(playerEntity->GetTransform()->GetPosition() - (Vector3(0.f, -7.f, 4.f) + camera.GetZoom())); //FIXME: Change to Entity Transform
			camera.SetLookAtPos(playerEntity->GetTransform()->GetPosition() - (Vector3(0.f, -14.f, 0.f) + camera.GetZoom()));
			camera.SetPitch(0);
			camera.SetYaw(0);
		}

		//CameraZoom
		if (mouse.scrollWheelValue > 0) {
			camera.ZoomIn();
			Input::ResetWheel();
		}
		if (mouse.scrollWheelValue < 0) {
			camera.ZoomOut();
			Input::ResetWheel();
		}
	}

	// UI FPS
	std::string str = std::to_string(fps);
	fpsFontText = std::wstring(str.begin(), str.end());

	if (!menuIsOn)
	{
		UpdateObjects(elapsedTime);
	}
	else
	{
		auto mouse = Input::GetMouseState();

		if (mouse.leftButton)
		{
			if ((mouse.x >= 320) && (mouse.x <= 450))
			{
				if ((mouse.y >= 300) && (mouse.y <= 370))
				{
					//menuIsOn = false;
					ExitGame();
				}	
				else if ((mouse.y >= 160) && (mouse.y <= 220))
				{
					menuIsOn = false;
				}
			}
			else if ((mouse.x >= 275) && (mouse.x <= 515))
			{
				if ((mouse.y >= 160) && (mouse.y <= 220))
				{
					menuIsOn = false;
				}			
			}
		}
	}

	elapsedTime;
}

void Game::UpdateObjects(float elapsedTime)
{
	auto mouse = Input::GetMouseState();
	auto keyboard = Input::GetKeyboardState();


	//NavMesh
	tracker.Update(mouse); // Player Component -> Player System
	if (tracker.leftButton == Mouse::ButtonStateTracker::PRESSED || tracker.leftButton == Mouse::ButtonStateTracker::HELD) {
		Vector3 destination = Raycast::GetPointOnGround(camera);
		navMesh->SetDestination(destination, camera);
	}
	navMesh->Move(); // Player Component -> Player System


	// check collisions
	vector<CollisionPtr> currentCollisions = collisionSystem->AllCollisions;
	vector<CollisionPtr> collisionsForEntity1 = collisionSystem->GetCollisionsForEntity(1);

	static Vector3 dir1(-1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	XMVECTORF32 collider1Color = DirectX::Colors::White;
	XMVECTORF32 collider2Color = DirectX::Colors::White;

	CollisionPtr collisionCup1WithRay, collisionCup2WithRay;

	//myEntity1->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir1, 1);
	myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2, 1);

	if (mouse.rightButton)
	{
		XMFLOAT3 posOnGround = Raycast::GetPointOnGround(camera);
		myEntity4->GetTransform()->SetPosition(Vector3(posOnGround.x, 0.5f, posOnGround.z));
	}

	BoundingBox octrTreeBounding = collisionSystem->GetOctTree()->Region->GetBounding();


	if (octrTreeBounding.Contains(colliderBoundingCup1->GetBounding()) != CONTAINS)
	{
		if (colliderBoundingCup1->GetCenter().x >= 0.0f)
			dir1.x = -1.0f;

		if (colliderBoundingCup1->GetCenter().x <= 0.0f)
			dir1.x = 1.0f;
	}

	if (octrTreeBounding.Contains(colliderBoundingCup2->GetBounding()) != CONTAINS)
	{
		if (colliderBoundingCup2->GetCenter().x >= 0.0f)
			dir2.x = -1.0f;

		if (colliderBoundingCup2->GetCenter().x <= 0.0f)
			dir2.x = 1.0f;
	}

	if (keyboard.M)
	{
		shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(camera));
		vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

		for each(shared_ptr<Collision> coll in collisionsWithRay)
		{
			if (coll->OriginObject->GetId() == colliderCup1->GetParent()->GetId())
				collisionCup1WithRay = coll;

			if (coll->OriginObject->GetId() == colliderCup2->GetParent()->GetId())
				collisionCup2WithRay = coll;
		}
	}

	// skinned model
	for (auto component : world->GetComponents<RenderableComponent>())
	{
		if (strcmp(component->GetParent()->GetName().c_str(),
			"Player") == 0)
		{
			component->_modelSkinned->GetAnimatorPlayer()->Update(elapsedTime); // -> Renderable System -> Iterate()
		}
	}
}

#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	// TODO: Add your rendering code here.

	renderableSystem->SentDeviceResources(m_deviceResources->GetRenderTargetView(), m_deviceResources->GetDepthStencilView());

	world->RefreshWorld();

	RenderObjects(context);

	context;

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::RenderObjects(ID3D11DeviceContext1 *context)
{
	DirectX::XMMATRIX cameraView = camera.GetViewMatrix(); // ?? FIXME
	DirectX::XMMATRIX cameraProjection = camera.GetProjectionMatrix();

	XMVECTORF32 collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding->CollisionKind);
	XMVECTORF32 collider2Color = Collision::GetCollisionColor(colliderCup2->ColliderBounding->CollisionKind);

	terrain->Update(collisionSystem->GetColliders());
	terrain->Draw(camera, m_roomTex);

	if (debugDraw) //REMOVE
		renderableSystem->DebugDrawAction->DrawOctTree(
			collisionSystem->GetOctTree(), cameraView, cameraProjection, debugDrawTreeRegions);

	uiSpriteBatch->Begin(); // TODO: UI System

	// show depth map
	//uiSpriteBatch->Draw(renderableSystem->_shadowMap->GetDepthMapSRV(), Vector2(450, 250), nullptr, Colors::White, 0.f, Vector2(0, 0), 0.3f);

	uiSpriteBatch->Draw(healthBarTex.Get(), healthBarPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.25f);

	uiSpriteBatch->Draw(healthBarHealthTex.Get(), healthBarHealthPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.25f);

	uiSpriteBatch->Draw(healthBarHeroTex.Get(), healthBarHeroPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.35f);

	uiSpriteBatch->Draw(fpsBarTex.Get(), fpsBarPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.15f);

	fpsFont->DrawString(uiSpriteBatch.get(), fpsFontText.c_str(),
		fpsFontPos, Colors::Black, 0.f, Vector2(0, 0));

	if (menuIsOn)
	{
		uiSpriteBatch->Draw(menuTex.Get(), menuPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), 0.6f);
	}

	uiSpriteBatch->End();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	context->RSSetState(0);


	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).

	//Audio
	audioSystem->Suspend();
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).

	//Audio
	audioSystem->Resume();
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	camera.SetScreenWidth(width);
	camera.SetScreenHeight(height);

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

void Game::OnNewAudioDevice()
{
	audioSystem->RetryAudio();
}

// Properties
void Game::GetDefaultSize(int& width, int& height)
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	int w = 800, h = 600;
	camera.SetScreenWidth(w);
	camera.SetScreenHeight(h);
	width = w;
	height = h;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()												// !!  CreateDevice()
{
	auto device = m_deviceResources->GetD3DDevice();									// m_d3dDevice.Get()  -> device
	auto context = m_deviceResources->GetD3DDeviceContext();							// m_d3dContext.Get() -> context

	// TODO: Initialize device dependent objects here (independent of window size).


	m_states = std::make_unique<CommonStates>(device); //REMOVE

	m_fxFactory = std::make_unique<EffectFactory>(device); //REMOVE

	InitializeObjects(device, context);

	device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()											// !! CreateResources()
{
	auto size = m_deviceResources->GetOutputSize();										// backBufferWidth/backBufferHeight - > size
	// TODO: Initialize windows-size dependent objects here.


	camera.SetPosition(0.0f, 0.0f, -2.0f); // TODO: Move to camera function
	camera.SetProjectionValues(XMConvertToRadians(70.f), float(size.right) / float(size.bottom), 0.01f, 100.f);
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	camera.SetZoom(XMFLOAT3(0.f, 0.f, 0.f));
}

void Game::InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context)
{
	terrain = std::make_shared<Terrain>();
	world = std::make_shared<World>();

	// Creation of systems ------------------------------------------------------------------
	audioSystem = std::make_shared<AudioSystem>();
	collisionSystem = std::make_shared<PhysicsSystem>(SCENE_CENTER, COLLISION_SCENE_RANGE, camera);
	renderableSystem = std::make_shared<RenderableSystem>(device, context, collisionSystem);
	lightSystem = std::make_shared<LightSystem>(renderableSystem->_fxFactory);
	
	// Adding systems to world ------------------------------------------------------------------
	world->AddSystem<PhysicsSystem>(collisionSystem, 0);
	world->AddSystem<LightSystem>(lightSystem, 1);
	world->AddSystem<AudioSystem>(audioSystem, 2);
	world->AddSystem<RenderableSystem>(renderableSystem, 3);
	
	/* 
		FILL WORLD OBJECT
	*/
	
	// Creation of entities ------------------------------------------------------------------
	myEntity1 = world->CreateEntity("Cup1");
	myEntity2 = world->CreateEntity("Cup2");
	myEntity3 = world->CreateEntity("Cup3");
	myEntity4 = world->CreateEntity("Cup4");
	myEntity5 = world->CreateEntity("BackgroundAudioEntity");
	myEntity6 = world->CreateEntity("Sound1AudioEntity");
	pointLightEntity1 = world->CreateEntity("PointLight1");
	pointLightEntity2 = world->CreateEntity("PointLight2");
	pointLightEntity3 = world->CreateEntity("PointLight3");
	spotLightEntity1 = world->CreateEntity("SpotLight1");
	directLightEntity1 = world->CreateEntity("DirectLight1");
	myEntityFloor = world->CreateEntity("FloorForShadows");
	playerEntity = world->CreateEntity("Player");

	// Creation of audio components ------------------------------------------------------------------
	myEntity5->AddComponent<AudioComponent>("Resources\\Audio\\In The End.wav");
	myEntity6->AddComponent<AudioComponent>("Resources\\Audio\\KnifeSlice.wav");

	// Creation of physics components ----------------------------------------------------------------
	myEntity1->AddComponent<PhysicsComponent>(Vector3::Zero, XMFLOAT3(.49f, 1.5f, 4.49f), false);
	myEntity2->AddComponent<PhysicsComponent>(Vector3::Zero, 0.7f, false);


	// Creation of renderable components
	myEntity1->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity2->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity3->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity4->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntityFloor->AddComponent<RenderableComponent>(L"FloorToRoom.cmo", &camera);
	playerEntity->AddComponent<RenderableComponent>(L"content\\Models\\Hero.fbx", &camera);
	//playerEntity->AddComponent<PhysicsComponent>(Vector3::Zero, XMFLOAT3(1.0f, 1.5f, 1.0f), false);

	// Creation of light components ------------------------------------------------------------------
	//pointLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), pointLightEntity1->GetTransform()->GetPosition(), 3.0f, true);
	//pointLightEntity2->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity2->GetTransform()->GetPosition(), 3.0f);
	//pointLightEntity3->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity3->GetTransform()->GetPosition(), 3.0f);
	//spotLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), 0.25f, spotLightEntity1->GetTransform()->GetPosition(), 0.75f, 10.0f);
	directLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(2.0f, -2.0f, 0.0f));
	// Setting up transform parameters of entities  --------------------------------------------------
	Vector3 scaleEntity1(0.1f, 0.1f, 0.1f), scaleEntity2(0.2f, 0.2f, 0.2f), scaleEntity3(0.3f, 0.3f, 0.3f), scaleEntity4(1.0f, 1.0f, 1.0f);
	myEntity1->GetTransform()->SetScale(scaleEntity1);
	myEntity1->GetTransform()->SetPosition(Vector3(7.0f, 1.25f, 4.0f));

	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->SetPosition(Vector3(6.0f, 0.2f, 6.0f));

	world->GetEntity(3)->GetTransform()->SetScale(scaleEntity3);
	myEntity3->GetTransform()->SetPosition(Vector3(0.0f, -1.5f, 0.0f));

	myEntity1->AddChild(myEntity3);

	world->GetEntity(4)->GetTransform()->SetScale(scaleEntity4);
	myEntity4->GetTransform()->SetPosition(Vector3(0.0f, -1.0f, -3.0f));

	myEntityFloor->GetTransform()->SetScale(Vector3(0.5f, 0.5f, 0.5f));
	myEntityFloor->GetTransform()->SetPosition(Vector3(-3.5f, 2.52f, -3.5f));

	playerEntity->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	playerEntity->GetTransform()->SetScale(Vector3(0.01f, 0.01f, 0.01f));

	// Setting up parameters of audio -- REMOVE
	for (auto component : world->GetComponents<AudioComponent>())
	{
		if (strcmp(component->GetParent()->GetName().c_str(),
			"BackgroundAudioEntity") == 0)
		{
			audioBackgroundSound = component;
			audioBackgroundSound->Loop = true;
			continue;
		}

		if (strcmp(component->GetParent()->GetName().c_str(),
			"Sound1AudioEntity") == 0)
		{
			audioSound1 = component;
			continue;
		}
	}

	// Setting up parameters of colliders ----------------------------------------------------------------

	for (auto physicsComponent : world->GetComponents<PhysicsComponent>())
	{
		if (strcmp(physicsComponent->GetParent()->GetName().c_str(), "Cup1") == 0)
		{
			colliderCup1 = physicsComponent;
			continue;
		}

		if (strcmp(physicsComponent->GetParent()->GetName().c_str(), "Cup2") == 0)
		{
			colliderCup2 = physicsComponent;
			continue;
		}
	}

	colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup1->ColliderBounding);
	colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup2->ColliderBounding);

	// Setting up positions of lights ----------------------------------------------------------------
	pointLightEntity1->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	pointLightEntity2->GetTransform()->SetPosition(Vector3(-2.0f, 0.0f, 2.0f));
	pointLightEntity3->GetTransform()->SetPosition(Vector3(2.0f, -1.0f, 1.0f));
	spotLightEntity1->GetTransform()->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
	directLightEntity1->GetTransform()->SetPosition(Vector3(0, 0, 0));
	// Setting up terrain tile map -------------------------------------------------------------------
	terrain->InitTileMap(context);
	

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"roomtexture.dds",
			nullptr, m_roomTex.ReleaseAndGetAddressOf())); //REMOVE
	//Setting up NavMesh ------------------------------------------------------------------------------
	navMesh = std::make_shared<NavMesh>(playerEntity->GetTransform());
	navMesh->terrain = this->terrain;

	//Setting up UI -----------------------------------------------------------------------------------
	uiSpriteBatch = std::make_shared<SpriteBatch>(context); // UI Component, UISystem->Initialize()

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources\\UISprites\\hp_bar.dds",
			nullptr,
			healthBarTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources\\UISprites\\hero.dds",
			nullptr,
			healthBarHeroTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources\\UISprites\\health.dds",
			nullptr,
			healthBarHealthTex.ReleaseAndGetAddressOf()));

	healthBarPos.x = 0.f;
	healthBarPos.y = 0.f;

	healthBarHeroPos.x = 0.f;
	healthBarHeroPos.y = 0.f;

	healthBarHealthPos.x = 135.f;
	healthBarHealthPos.y = 30.f;

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources\\UISprites\\fpsbar.dds",
			nullptr,
			fpsBarTex.ReleaseAndGetAddressOf()));

	fpsBarPos.x = 710.0f;
	fpsBarPos.y = -5.0f;

	fpsFont = std::make_unique<SpriteFont>(device, L"Resources\\Fonts\\fpsFont.spritefont");

	fpsFontPos.x = 710.0f;
	fpsFontPos.y = 10.0f;
	fpsFontText = std::wstring(L"60");

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Resources\\UISprites\\Menu.dds",
			nullptr,
			menuTex.ReleaseAndGetAddressOf()));

	menuPos.x = 250.0f;
	menuPos.y = 100.0f;

	menuIsOn = false;


	// Initialization of systems ---------------------------------------------------------------------
	world->InitializeSystem<AudioSystem>();
	world->InitializeSystem<PhysicsSystem>();
	world->InitializeSystem<RenderableSystem>();
	world->InitializeSystem<LightSystem>();


	// ----------------------   AFTER INITIALIZATION   -----------------------------------------------

	//Setting up skinned model -----------------------------------------------------------------------
	for (auto component : world->GetComponents<RenderableComponent>())
	{
		if (strcmp(component->GetParent()->GetName().c_str(),
			"Player") == 0)
		{
			component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Walk.fbx", "Walk");
			component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_HipHop.fbx", "HipHop");
			component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Dance.fbx", "Dance");
		}
	}

	//world->RefreshWorld();
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_fxFactory.reset();

	myEntity1->Model.reset();
	myEntity2->Model.reset();
	myEntity3->Model.reset();
	myEntity4->Model.reset();

	//m_room.reset();
	//m_roomTex.Reset();
	//terrain.block.reset();

	myEntityFloor->Model.reset();
	m_plane.reset();
	m_planeTex.Reset();

	uiSpriteBatch.reset();
	healthBarTex.Reset();
	healthBarHealthTex.Reset();
	healthBarHeroTex.Reset();
	fpsFont.reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion