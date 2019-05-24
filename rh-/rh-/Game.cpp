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
	const XMVECTORF32 ROOM_BOUNDS = { 1.f, 0.f, 1.f, 0.f };
	const float COLLISION_SCENE_RANGE = 15.0f;
	const Vector3 SCENE_CENTER = Vector3::Zero;
	const float ROTATION_GAIN = 0.008f;
	const float MOVEMENT_GAIN = 0.07f;

	const XMVECTORF32 PLANE_BOUNDS = { 1.5f, 1.5f, 1.5f, 0.f };
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


	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	/*m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);*/

	inputEntity = std::make_shared<Entity>();
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
	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter)
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

			if (*iter == moveFor)
			{
				mSkinModelTransform->Translate((Vector3(0.0f, 0.0f, 1.0f))*elapsedTime * 0.001f, elapsedTime);
				mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
				mSkinModel->SetInMove(true);
				mSkinModel->GetAnimatorPlayer()->SetDirection(true);
			}

			if (*iter == moveBac)
			{
				mSkinModelTransform->Translate((Vector3(0.0f, 0.0f, -1.0f))*elapsedTime * 0.001f, -elapsedTime);
				mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
				mSkinModel->SetInMove(true);
				mSkinModel->GetAnimatorPlayer()->SetDirection(false);
			}

			if (*iter == moveLeft)
			{
				mSkinModelTransform->Rotate(Vector3(0, 1, 0), XMConvertToRadians(150.f * elapsedTime));
			}

			if (*iter == moveRight)
			{
				mSkinModelTransform->Rotate(Vector3(0, 1, 0), XMConvertToRadians(-150.f * elapsedTime));
			}

			if (*iter == special1)
			{
				mSkinModel->GetAnimatorPlayer()->StartClip("HipHop");
				mSkinModel->SetInMove(true);
				mSkinModel->GetAnimatorPlayer()->SetDirection(true);
			}

			if (*iter == special2)
			{
				mSkinModel->GetAnimatorPlayer()->StartClip("Dance");
				mSkinModel->SetInMove(true);
				mSkinModel->GetAnimatorPlayer()->SetDirection(true);
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

		if (pushedKeysActions.size() == 0 && !navMesh->isMoving)
		{
			mSkinModel->GetAnimatorPlayer()->StartClip("Idle");
			mSkinModel->SetInMove(true);
		}

		if (navMesh->isMoving) {
			mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}
	}

	//Audio
	audioSystem->Iterate();

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
			camera.SetPosition(mSkinModelTransform->GetPosition() - (Vector3(0.f, -7.f, 4.f) + camera.GetZoom()));
			camera.SetLookAtPos(mSkinModelTransform->GetPosition() - (Vector3(0.f, -14.f, 0.f) + camera.GetZoom()));
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
	static shared_ptr<Entity> selectedCollider;


	//NavMesh
	tracker.Update(mouse);
	if (tracker.leftButton == Mouse::ButtonStateTracker::PRESSED || tracker.leftButton == Mouse::ButtonStateTracker::HELD) {
		Vector3 destination = Raycast::GetPointOnGround(camera);
		navMesh->SetDestination(destination);
	}
	navMesh->Move();

	/*if (destination != Vector3::Zero) {
		if (!XMVector3NearEqual(destination, mSkinModelTransform->GetPosition(), Vector3(.001f, .001f, .001f))){
			mSkinModelTransform->SetPosition(mSkinModelTransform->GetPosition() + step);

			// RED light follow player
			Vector3 temppos = pointLightEntity1->GetTransform()->GetPosition() + step;
			pointLightEntity1->GetTransform()->SetPosition(Vector3(temppos.x, 0.0f, temppos.z));

			mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}
		else {
			step = Vector3(0.f, 0.f, 0.f);
		}
	}*/

	// check collisions
	//collisionSystem->Iterate();
	static Vector3 dir1(-1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	XMVECTORF32 collider1Color = DirectX::Colors::White;
	XMVECTORF32 collider2Color = DirectX::Colors::White;

	CollisionPtr collisionEntity1WithWall = Collision::CheckCollision(colliderCup1, colliderSceneWall);
	CollisionPtr collisionEntity2WithWall = Collision::CheckCollision(colliderCup2, colliderSceneWall);
	CollisionPtr collisionBetweenCups = Collision::CheckCollision(colliderCup1, colliderCup2);
	CollisionPtr collisionCup1WithRay, collisionCup2WithRay;

	myEntity1->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir1, 1);
	myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2, 1);

	if (mouse.rightButton)
	{
		XMFLOAT3 posOnGround = Raycast::GetPointOnGround(camera);
		//myEntity4->GetTransform()->SetPosition(posOnGround / myEntity4->GetTransform()->GetScale());
		myEntity4->GetTransform()->SetPosition(posOnGround);
		//myEntity4->AddComponent<AudioComponent>("Resources\\Audio\\In The End.wav");
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

	if (colliderBoundingCup1->GetCenter().x >= 0.0f && collisionEntity1WithWall->CollisionKind != CONTAINS)
		dir1.x = -1.0f;

	if (colliderBoundingCup1->GetCenter().x <= 0.0f && collisionEntity1WithWall->CollisionKind != CONTAINS)
		dir1.x = 1.0f;

	if (colliderBoundingCup2->GetCenter().x >= 0.0f && collisionEntity2WithWall->CollisionKind != CONTAINS)
		dir2.x = -1.0f;

	if (colliderBoundingCup2->GetCenter().x <= 0.0f && collisionEntity2WithWall->CollisionKind != CONTAINS)
		dir2.x = 1.0f;

	// skinned model
	mSkinModel->GetAnimatorPlayer()->Update(elapsedTime);
	if (mSkinModel->GetInMove())
	{
		mSkinModel->GetAnimatorPlayer()->ResumeClip();
	}
	else
	{
		mSkinModel->GetAnimatorPlayer()->PauseClip();
	}

	world->RefreshWorld();

	//lightSystem->Iterate();

	//billboarding
	//planeWorld = Matrix::CreateBillboard(planePos, camera.GetPositionVector(), camera.GetUpVector());
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

	RenderObjects(context);

	context;

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::RenderObjects(ID3D11DeviceContext1 *context)
{
	DirectX::XMMATRIX cameraView = camera.GetViewMatrix();
	DirectX::XMMATRIX cameraProjection = camera.GetProjectionMatrix();
	

	XMVECTORF32 collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding->CollisionKind);
	XMVECTORF32 collider2Color = Collision::GetCollisionColor(colliderCup2->ColliderBounding->CollisionKind);

	// room
	//m_room->Draw(Matrix::Identity, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_roomTex.Get());

	terrain->Draw(camera, m_roomTex);

	renderableSystem->Iterate();

	if (debugDraw)
		renderableSystem->DebugDrawAction->DrawOctTree(
			collisionSystem->GetOctTree(), cameraView, cameraProjection, debugDrawTreeRegions);

	// skinned model
	mSkinModel->DrawModel(context, *m_states, mSkinModelTransform->GetTransformMatrix(), cameraView, cameraProjection);

	// billboarding
	//m_plane->Draw(planeWorld, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
	m_plane2->Draw(planeWorld2, cameraView, cameraProjection, Colors::White, m_planeTex.Get());
	m_plane3->Draw(planeWorld3, cameraView, cameraProjection, Colors::White, m_planeTex.Get());
	m_plane4->Draw(planeWorld4, cameraView, cameraProjection, Colors::White, m_planeTex.Get());


	uiSpriteBatch->Begin();

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


	m_states = std::make_unique<CommonStates>(device);

	m_fxFactory = std::make_unique<EffectFactory>(device);

	InitializeObjects(device, context);

	device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()											// !! CreateResources()
{
	auto size = m_deviceResources->GetOutputSize();										// backBufferWidth/backBufferHeight - > size
	// TODO: Initialize windows-size dependent objects here.


	camera.SetPosition(0.0f, 0.0f, -2.0f); // TODO
	camera.SetProjectionValues(XMConvertToRadians(70.f), float(size.right) / float(size.bottom), 0.01f, 100.f);
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	camera.SetZoom(XMFLOAT3(0.f, 0.f, 0.f));
}

void Game::InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context)
{
	m_world = Matrix::Identity;

	terrain = std::make_shared<Terrain>();

	world = std::make_shared<World>();

	//entityManager = std::make_shared<EntityManager>();
	//componentFactory = std::make_shared<ComponentFactory>(entityManager);

	renderableSystem = std::make_shared<RenderableSystem>(device, context);

	lightSystem = std::make_shared<LightSystem>(renderableSystem->_fxFactory);

	collisionSystem = std::make_shared<PhysicsSystem>(Vector3::Zero, ROOM_BOUNDS[0]);


	world->AddSystem<PhysicsSystem>(collisionSystem, 0);
	world->AddSystem<LightSystem>(lightSystem, 1);
	world->AddSystem<RenderableSystem>(renderableSystem, 2);
	
	sceneWallEntity = world->CreateEntity("SceneWall");
	myEntity1 = world->CreateEntity("Cup1");
	myEntity2 = world->CreateEntity("Cup2");
	myEntity3 = world->CreateEntity("Cup3");
	myEntity4 = world->CreateEntity("Cup4");

	myEntity1->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity2->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity3->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	myEntity4->AddComponent<RenderableComponent>(L"cup.cmo", &camera);


	Vector3 scaleEntity1(0.1f, 0.1f, 0.1f), scaleEntity2(0.2f, 0.2f, 0.2f), scaleEntity3(0.3f, 0.3f, 0.3f), scaleEntity4(0.35f, 0.35f, 0.35f);

	//myEntity1->GetTransform()->SetScale(scaleEntity1);
	//myEntity2->GetTransform()->SetScale(scaleEntity1);
	//myEntity3->GetTransform()->SetScale(scaleEntity1);
	//myEntity4->GetTransform()->SetScale(scaleEntity1);
	
	world->InitializeSystem<RenderableSystem>();
	world->InitializeSystem<PhysicsSystem>();
	world->InitializeSystem<LightSystem>();

	// pointLightEntity1 - RED light follow player
	pointLightEntity1 = world->CreateEntity("PointLight1");
	pointLightEntity2 = world->CreateEntity("PointLight2");
	pointLightEntity3 = world->CreateEntity("PointLight3");
	spotLightEntity1 = world->CreateEntity("SpotLight1");

	//myEntity1->Model =
		//DirectX::Model::CreateFromCMO(device, L"cup.cmo", m_fxFactory);

	pointLightEntity1->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	pointLightEntity2->GetTransform()->SetPosition(Vector3(-2.0f, 0.0f, 2.0f));
	pointLightEntity3->GetTransform()->SetPosition(Vector3(2.0f, -1.0f, 1.0f));
	spotLightEntity1->GetTransform()->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	// lightComponent - RED light follow player
	pointLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), pointLightEntity1->GetTransform()->GetPosition(), 3.0f, true);
	pointLightEntity2->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity2->GetTransform()->GetPosition(), 3.0f);
	pointLightEntity3->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity3->GetTransform()->GetPosition(), 3.0f);
	spotLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), 0.25f, spotLightEntity1->GetTransform()->GetPosition(), 0.75f, 10.0f);

	//world->InitializeSystem<RenderableSystem>();

	//myEntity1->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	//myEntity1->AddComponent<RenderableComponent>(L"cup.cmo", camera);


	//myEntity1->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity1->GetTransform()->SetScale(scaleEntity1);
	myEntity1->GetTransform()->SetPosition(Vector3(-6.0f, -6.0f, 6.0f));
	myEntity1->Update();

	//myEntity2->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->SetPosition(Vector3(6.0f, -6.0f, 6.0f));
	myEntity2->Update();

	//myEntity3->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	world->GetEntity(3)->GetTransform()->SetScale(scaleEntity3);
	
	myEntity3->GetTransform()->SetPosition(Vector3(0.0f, -1.5f, 0.0f));
	myEntity3->Update();

	//myEntity4->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	world->GetEntity(4)->GetTransform()->SetScale(scaleEntity4);
	myEntity4->GetTransform()->SetPosition(Vector3(0.0f, -1.0f, -3.0f));
	myEntity4->Update();

	myEntity1->AddChild(myEntity3);

	initialBoundingEntity1Size = XMFLOAT3(0.4f, 0.4f, 0.4f);
	initialBoundingEntity2Size = XMFLOAT3(0.4f, 0.4f, 0.4f);
	initialBounding1Radius = 0.3f;
	initialBounding2Radius = 0.7f;

	collisionSystem = std::make_shared<PhysicsSystem>(SCENE_CENTER, COLLISION_SCENE_RANGE);

	colliderSceneWall = std::make_shared<PhysicsComponent>(AABB);
	colliderSceneWall->SetParent(sceneWallEntity);
	colliderBoundingSceneWall = std::dynamic_pointer_cast<ColliderAABB>(colliderSceneWall->ColliderBounding);
	colliderBoundingSceneWall->SetExtents(XMFLOAT3(COLLISION_SCENE_RANGE / 2.0f, COLLISION_SCENE_RANGE / 2.0f, COLLISION_SCENE_RANGE / 2.0f));
	//collisionSystem->InsertComponent(colliderSceneWall);

	colliderCup1 = std::make_shared<PhysicsComponent>(AABB);
	colliderCup1->SetParent(myEntity1);
	colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup1->ColliderBounding);
	colliderBoundingCup1->SetExtents(initialBoundingEntity1Size);
	collisionSystem->InsertComponent(colliderCup1);
	//colliderCup1->SetIsEnabled(false);

	colliderCup2 = std::make_shared<PhysicsComponent>(Sphere);
	colliderCup2->SetParent(myEntity2);
	colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup2->ColliderBounding);
	colliderBoundingCup2->SetRadius(initialBounding2Radius);
	collisionSystem->InsertComponent(colliderCup2);

	//m_room = GeometricPrimitive::CreateBox(context,
	//	XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
	//	false, true);

	terrain->InitTileMap(context);
	collisionSystem->Initialize();

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"roomtexture.dds",
			nullptr, m_roomTex.ReleaseAndGetAddressOf()));



	//skinned model
	mSkinModel = std::make_shared<ModelSkinned>(device, "content\\Models\\Hero.fbx", context);
	mSkinModel->AddAnimationClip("content\\Models\\Hero_Walk.fbx", "Walk");
	mSkinModel->AddAnimationClip("content\\Models\\Hero_HipHop.fbx", "HipHop");
	mSkinModel->AddAnimationClip("content\\Models\\Hero_Dance.fbx", "Dance");

	mSkinModelTransform = std::make_shared<Transform>();
	mSkinModelTransform->SetScale(Vector3(0.01f, 0.01f, 0.01f));
	mSkinModelTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	//billboarding
	m_plane = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(PLANE_BOUNDS[0], PLANE_BOUNDS[1], PLANE_BOUNDS[2]),
		true, true);
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"cat.dds",
			nullptr, m_planeTex.ReleaseAndGetAddressOf()));
	planeWorld = m_world;


	m_plane2 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(PLANE_BOUNDS[0], PLANE_BOUNDS[1], PLANE_BOUNDS[2]),
		true, true);
	planeWorld2 = m_world;


	m_plane3 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(PLANE_BOUNDS[0], PLANE_BOUNDS[1], PLANE_BOUNDS[2]),
		true, true);
	planeWorld3 = m_world;

	m_plane4 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(PLANE_BOUNDS[0], PLANE_BOUNDS[1], PLANE_BOUNDS[2]),
		true, true);
	planeWorld4 = m_world;


	//planeWorld = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	planeWorld2 = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	planeWorld3 = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	planeWorld4 = XMMatrixScaling(0.2f, 0.2f, 0.2f);

	//planePos = Vector3(-2.0f, 0.0f, -2.0f);
	//planeWorld.CreateTranslation(planePos);

	//planeWorld = planeWorld * XMMatrixTranslation(-2.0f, 0.0f, -2.0f);
	planeWorld2 = planeWorld2 * XMMatrixTranslation(-2.0f, 0.0f, 2.0f);
	planeWorld3 = planeWorld3 * XMMatrixTranslation(2.0f, -1.0f, 1.0f);
	planeWorld4 = planeWorld4 * XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	//Audio
	audioSystem = std::make_shared<AudioSystem>();
	//audioSystem->Initialize();
	//audioBackgroundSound = std::dynamic_pointer_cast<AudioComponent>(entityManager->GetEntityComponentsOfType(entityManager->GetEntity("BackgroundAudioEntity")->GetId(), ComponentType("Audio"))[0]);
	//audioSound1 = std::dynamic_pointer_cast<AudioComponent>(entityManager->GetEntityComponentsOfType(entityManager->GetEntity("Sound1AudioEntity")->GetId(), ComponentType("Audio"))[0]);

	//NavMesh
	navMesh = std::make_shared<NavMesh>(mSkinModelTransform);
	navMesh->terrain = this->terrain;

	//UI
	uiSpriteBatch = std::make_shared<SpriteBatch>(context);


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