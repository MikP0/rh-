//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace
{
	const XMVECTORF32 ROOM_BOUNDS = { 8.f, 6.f, 12.f, 0.f };
	const float ROTATION_GAIN = 0.008f;
	const float MOVEMENT_GAIN = 0.07f;

	const XMVECTORF32 PLANE_BOUNDS = { 1.5f, 1.5f, 1.5f, 0.f };
}


Game::Game() noexcept(false) : m_pitch(0), m_yaw(0)
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

	// TODO: Add your game logic here.


	// INPUT
	auto mouse = Input::GetMouseState();
	/*auto mouse = m_mouse->GetState();
	auto keyboard = m_keyboard->GetState();*/
	Vector3 tempCamera;
	Vector3 move = Vector3::Zero;

	if (mouse.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f)
			* ROTATION_GAIN;

		m_pitch -= delta.y;
		m_yaw -= delta.x;

		// limit pitch to straight up or straight down
		// with a little fudge-factor to avoid gimbal lock
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

	/*m_mouse->SetMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	if (keyboard.Escape)
		ExitGame();

	if (keyboard.PageUp || keyboard.Space)
		move.y += 1.f;

	if (keyboard.PageDown || keyboard.LeftControl)
		move.y -= 1.f;

	if (keyboard.Left || keyboard.A)
		move.x += 1.f;

	if (keyboard.Right || keyboard.D)
		move.x -= 1.f;

	if (keyboard.Up || keyboard.W)
		move.z += 1.f;

	if (keyboard.Down || keyboard.S)
		move.z -= 1.f;*/

	std::vector<actionList> pushedKeysActions = Input::GetActions();
	Input::SetMouseMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter)
	{
		// zmiana MouseMode tutaj z udzia�em InputSystemu spowalnia renderowanie przy obracaniu (nie wiem czemu)
		//inputSystem->SetMouseMode(*iter == anchorRotation ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

		if (*iter == closeWindow)
			ExitGame();

		/*if (*iter == up)
			move.y += 1.f;

		if (*iter == down)
			move.y -= 1.f;*/

		if (*iter == actionList::left)
			move.x += 1.f;

		if (*iter == actionList::right)
			move.x -= 1.f;

		if (*iter == actionList::forward)
			move.z += 1.f;

		if (*iter == actionList::backward)
			move.z -= 1.f; 

		if (*iter == actionList::up)
		{
			mSkinModel->character_world = mSkinModel->character_world * XMMatrixTranslation(0.0f, 0.0f, 0.03f);
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}

		if (*iter == down)
		{
			mSkinModel->character_world = mSkinModel->character_world * XMMatrixTranslation(0.0f, 0.0f, -0.03f);
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(false);
		}
	}

	if (pushedKeysActions.size() == 0)
	{
		mSkinModel->SetInMove(false);
	}

	move = Vector3::Transform(move, Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f));
	move *= MOVEMENT_GAIN;
	tempCamera = camera.GetPositionVector();
	tempCamera += move;
	camera.SetPosition(tempCamera);

	Vector3 halfBound = (Vector3(ROOM_BOUNDS.v) / Vector3(2.f)) - Vector3(0.1f, 0.1f, 0.1f);

	camera.SetPosition(Vector3::Min(camera.GetPositionVector(), halfBound));
	camera.SetPosition(Vector3::Max(camera.GetPositionVector(), -halfBound));
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	////////

	UpdateObjects(elapsedTime);


	elapsedTime;
}

void Game::UpdateObjects(float elapsedTime)
{
	// check collisions
	static Vector3 dir1(-1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	XMVECTORF32 collider1Color = DirectX::Colors::White;
	XMVECTORF32 collider2Color = DirectX::Colors::White;

	collisionSystem->UpdateCollidersPositions();
	CollisionPtr collisionEntity1WithWall = collisionSystem->CheckCollision(colliderCup1, colliderSceneWall);
	CollisionPtr collisionEntity2WithWall = collisionSystem->CheckCollision(colliderCup2, colliderSceneWall);
	CollisionPtr collisionBetweenCups = collisionSystem->CheckCollision(colliderCup1, colliderCup2);

	Vector3 scaleEntity1(0.5f, 0.5f, 0.5f), scaleEntity2(0.2f, 0.2f, 0.2f), scaleEntity3(0.3f, 0.3f, 0.3f);
	myEntity1->GetTransform()->SetScale(scaleEntity1);
	myEntity1->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir1);
	myEntity1->Update();
	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2);
	myEntity2->Update();
	myEntity3->GetTransform()->SetScale(scaleEntity3);
	myEntity3->Update();

	if (colliderBoundingCup1->Bounding.Center.x >= 0.0f && collisionEntity1WithWall->CollisionKind != CONTAINS)
		dir1.x = -1.0f;

	if (colliderBoundingCup1->Bounding.Center.x <= 0.0f && collisionEntity1WithWall->CollisionKind != CONTAINS)
		dir1.x = 1.0f;

	if (colliderBoundingCup2->Bounding.Center.x >= 0.0f && collisionEntity2WithWall->CollisionKind != CONTAINS)
		dir2.x = -1.0f;

	if (colliderBoundingCup2->Bounding.Center.x <= 0.0f && collisionEntity2WithWall->CollisionKind != CONTAINS)
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


	//billboarding
	planeWorld = Matrix::CreateBillboard(planePos, camera.GetPositionVector(), camera.GetUpVector());
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

	//
	//camera.AdjustPosition(0.0f, 0.01f, 0.0f);
	//camera.SetLookAtPos(myEntity.GetTransform()->GetPosition());
	//
	//myEntity.GetTransform()->SetPosition(Vector3(0.2f, 1.0f, 1.5f));
	//myEntity.GetTransform()->SetScale(Vector3(0.2f, 1.0f, 1.5f));
	//

	RenderObjects(context);

	context;

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::RenderObjects(ID3D11DeviceContext1 *context)
{
	XMVECTORF32 collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding->CollisionKind);
	XMVECTORF32 collider2Color = Collision::GetCollisionColor(colliderCup2->ColliderBounding->CollisionKind);

	// room
	m_room->Draw(Matrix::Identity, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_roomTex.Get());

	// cups
	m_boundingEntity1 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(colliderBoundingCup1->Bounding.Extents.x * 2.0f,
			colliderBoundingCup1->Bounding.Extents.y * 2.0f,
			colliderBoundingCup1->Bounding.Extents.z * 2.0f),
		false, true);

	/*m_boundingEntity1 = GeometricPrimitive::CreateSphere(context,
		colliderBoundingCup1->Bounding.Radius * 2.0f,
		16, false, true);*/

		/*m_boundingEntity2 = GeometricPrimitive::CreateBox(context,
			XMFLOAT3(colliderBoundingCup2->Bounding.Extents.x * 2.0f,
				colliderBoundingCup2->Bounding.Extents.y * 2.0f,
				colliderBoundingCup2->Bounding.Extents.z * 2.0f),
			false, true);*/

	m_boundingEntity2 = GeometricPrimitive::CreateSphere(
		context, colliderBoundingCup2->Bounding.Radius  * 2.0f,
		16, false, true);

	dxmath::Matrix boundingMatrix1 = dxmath::Matrix::CreateTranslation(colliderBoundingCup1->Bounding.Center);
	dxmath::Matrix boundingMatrix2 = dxmath::Matrix::CreateTranslation(colliderBoundingCup2->Bounding.Center);


	myEntity1->Model->Draw(context, *m_states, myEntity1->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	myEntity2->Model->Draw(context, *m_states, myEntity2->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	myEntity3->Model->Draw(context, *m_states, myEntity3->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());

	m_boundingEntity1->Draw(boundingMatrix1, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider1Color, nullptr, true);
	m_boundingEntity2->Draw(boundingMatrix2, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider2Color, nullptr, true);

	// skinned model
	mSkinModel->DrawModel(context, *m_states, false, false, camera.GetViewMatrix(), camera.GetProjectionMatrix());

	// billboarding
	m_plane->Draw(planeWorld, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
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
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
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

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	//camera.SetProjectionValues(XM_PI / 4.f, float(size.right) / float(size.bottom), 0.1f, 100.f);
	camera.SetProjectionValues(XMConvertToRadians(70.f), float(size.right) / float(size.bottom), 0.01f, 100.f);
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
}

void Game::InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context)
{
	m_world = Matrix::Identity;

	entityManager = std::make_unique<EntityManager>();

	sceneWallEntity = entityManager->GetEntity(entityManager->CreateEntity());
	myEntity1 = entityManager->GetEntity(entityManager->CreateEntity());
	myEntity2 = entityManager->GetEntity(entityManager->CreateEntity());
	myEntity3 = entityManager->GetEntity(entityManager->CreateEntity());

	sceneWallEntity->SetWorldMatrix(m_world);

	myEntity1->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity1->SetWorldMatrix(m_world);
	myEntity1->GetTransform()->SetPosition(Vector3(-1.0f, 0.0f, 0.0f));

	myEntity2->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity2->SetWorldMatrix(m_world);
	myEntity2->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));

	myEntity3->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity3->SetWorldMatrix(m_world);
	myEntity3->GetTransform()->SetPosition(Vector3(0.0f, -1.5f, 0.0f));

	myEntity1->AddChild(myEntity3);

	initialBoundingEntity1Size = XMFLOAT3(0.3f, 0.3f, 0.3f);
	initialBoundingEntity2Size = XMFLOAT3(0.8f, 0.8f, 0.8f);
	initialBounding1Radius = 0.3f;
	initialBounding2Radius = 0.8f;

	collisionSystem = std::make_shared<PhysicsSystem>();

	colliderSceneWall = std::make_shared<PhysicsComponent>(AABB);
	colliderSceneWall->SetParent(sceneWallEntity);
	colliderBoundingSceneWall = std::dynamic_pointer_cast<ColliderAABB>(colliderSceneWall->ColliderBounding);
	colliderBoundingSceneWall->Bounding.Extents = XMFLOAT3(ROOM_BOUNDS[0] / 2.0f, ROOM_BOUNDS[1] / 2.0f, ROOM_BOUNDS[2] / 2.0f);
	collisionSystem->InsertComponent(colliderSceneWall);

	colliderCup1 = std::make_shared<PhysicsComponent>(AABB);
	//colliderCup1 = std::make_shared<PhysicsComponent>(Sphere);
	colliderCup1->SetParent(myEntity1);
	colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup1->ColliderBounding);
	//colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup1->ColliderBounding);
	colliderBoundingCup1->Bounding.Extents = initialBoundingEntity1Size;
	//colliderBoundingCup1->Bounding.Radius = initialBounding1Radius;
	collisionSystem->InsertComponent(colliderCup1);

	//colliderCup2 = std::make_shared<PhysicsComponent>(AABB);
	colliderCup2 = std::make_shared<PhysicsComponent>(Sphere);
	colliderCup2->SetParent(myEntity2);
	//colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup2->ColliderBounding);
	colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup2->ColliderBounding);
	//colliderBoundingCup2->Bounding.Extents = initialBoundingEntity2Size;
	colliderBoundingCup2->Bounding.Radius = initialBounding2Radius;
	collisionSystem->InsertComponent(colliderCup2);

	m_room = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
		false, true);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"roomtexture.dds",
			nullptr, m_roomTex.ReleaseAndGetAddressOf()));

	//skinned model
	mSkinModel = std::make_shared<ModelSkinned>(m_world, device, context, "Content\\Models\\theHeroF.dae");


	//billboarding
	m_plane = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(PLANE_BOUNDS[0], PLANE_BOUNDS[1], PLANE_BOUNDS[2]),
		true, true);
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"cat.dds",
			nullptr, m_planeTex.ReleaseAndGetAddressOf()));
	planeWorld = m_world;
	planePos = Vector3(2.0f, 0.f, 4.0f);
	planeWorld.CreateTranslation(planePos);
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_fxFactory.reset();

	myEntity1->Model.reset();
	myEntity2->Model.reset();

	m_room.reset();
	m_roomTex.Reset();


	m_plane.reset();
	m_planeTex.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion