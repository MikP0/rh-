//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "System.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace
{
	const XMVECTORF32 ROOM_BOUNDS = { 15.f, 6.f, 12.f, 0.f };
	const float ROTATION_GAIN = 0.008f;
	const float MOVEMENT_GAIN = 0.07f;

	const XMVECTORF32 PLANE_BOUNDS = { 1.5f, 1.5f, 1.5f, 0.f };
}

/*
	Tymczasowa zmienna !!!!!!
*/
bool angel = true;
Mouse::ButtonStateTracker tracker;
Vector3 destination = Vector3::Zero;
Vector3 step = Vector3::Zero;

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
	
	// TODO: Add your game logic here.


	// INPUT
	auto mouse = Input::GetMouseState();
	/*auto mouse = m_mouse->GetState();
	auto keyboard = m_keyboard->GetState();*/
	Vector3 tempCamera;
	Vector3 move = Vector3::Zero;

	/*if (mouse.positionMode == Mouse::MODE_RELATIVE)
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
	}*/

	if (mouse.scrollWheelValue > 0) {
		camera.ZoomIn();
		Input::ResetWheel();

	}
	if (mouse.scrollWheelValue < 0) {
		camera.ZoomOut();
		Input::ResetWheel();
	}


	std::vector<actionList> pushedKeysActions = Input::GetActions();
	Input::SetMouseMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter)
	{
		// zmiana MouseMode tutaj z udzialem InputSystemu spowalnia renderowanie przy obracaniu (nie wiem czemu)
		//inputSystem->SetMouseMode(*iter == anchorRotation ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

		if (*iter == closeWindow)
			ExitGame();

		if (*iter == up)
			move.y += 1.f;

		if (*iter == down)
			move.y -= 1.f;

		if (*iter == actionList::left)
			move.x += 1.f;

		if (*iter == actionList::right)
			move.x -= 1.f;

		if (*iter == actionList::forward) {
			move.z += 1.f;
		}

		if (*iter == actionList::backward) {
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


		/*if (*iter == moveLeft)
		{
			mSkinModelTransform->Rotate(Vector3(0, 1, 0), XMConvertToRadians(90.f));
			mSkinModelTransform->Translate(Vector3(0.03f, 0.0f, 0.0f));
			mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}

		if (*iter == moveRight)
		{
			mSkinModelTransform->Rotate(Vector3(0, 1, 0), XMConvertToRadians(-90.f));
			mSkinModelTransform->Translate(Vector3(-0.03f, 0.0f, 0.0f));
			mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
			mSkinModel->SetInMove(true);
			mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}*/


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
		}
	}

	if (pushedKeysActions.size() == 0 && step == Vector3(0.f, 0.f, 0.f))
	{
		mSkinModel->GetAnimatorPlayer()->StartClip("Idle");
		mSkinModel->SetInMove(true);
	}

	tracker.Update(mouse);
	if (tracker.leftButton == Mouse::ButtonStateTracker::PRESSED) {
		destination = Raycast::GetPointOnGround(camera);
		//destination.y = -3.f;	// FLOOR LEVEL
		//destination.y = mSkinModelTransform->GetPosition().y;
		step = destination - mSkinModelTransform->GetPosition();
		step = step / 100;

		
		float dot = mSkinModelTransform->GetTransformMatrix().Forward().x * (destination - mSkinModelTransform->GetPosition()).x + mSkinModelTransform->GetTransformMatrix().Forward().z * (destination - mSkinModelTransform->GetPosition()).z;
		float cross = mSkinModelTransform->GetTransformMatrix().Forward().x * (destination - mSkinModelTransform->GetPosition()).z - mSkinModelTransform->GetTransformMatrix().Forward().z * (destination - mSkinModelTransform->GetPosition()).x;
		float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
		mSkinModelTransform->Rotate(Vector3(0, 1, 0), XMConvertToRadians(-fAngle));
	
	}

	//move = Vector3::Transform(move, Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f));
	//move *= MOVEMENT_GAIN;
	//tempCamera = camera.GetPositionVector();
	//tempCamera += move;
	//camera.SetPosition(tempCamera);

	Vector3 halfBound = (Vector3(ROOM_BOUNDS.v) / Vector3(2.f)) - Vector3(0.1f, 0.1f, 0.1f);

	//camera.SetPosition(Vector3::Min(camera.GetPositionVector(), halfBound));
	//camera.SetPosition(Vector3::Max(camera.GetPositionVector(), -halfBound));
	//tempCamera = camera.GetPositionVector();
	//tempCamera = tempCamera * zoom;
	camera.SetPosition(mSkinModelTransform->GetPosition() - (Vector3(0.f, -7.f, 4.f) + camera.GetZoom()));
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	camera.SetLookAtPos(mSkinModelTransform->GetPosition() - (Vector3(0.f, -14.f, 0.f) + camera.GetZoom()));
	////////

	//Audio
	audioSystem->Iterate();

	UpdateObjects(elapsedTime);


	elapsedTime;
}

void Game::UpdateObjects(float elapsedTime)
{
	auto mouse = Input::GetMouseState();
	static shared_ptr<Entity> selectedCollider;

	if (destination != Vector3::Zero) {
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
	}

	// check collisions
	collisionSystem->Iterate();
	static Vector3 dir1(-1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	XMVECTORF32 collider1Color = DirectX::Colors::White;
	XMVECTORF32 collider2Color = DirectX::Colors::White;

	CollisionPtr collisionEntity1WithWall = Collision::CheckCollision(colliderCup1, colliderSceneWall);
	CollisionPtr collisionEntity2WithWall = Collision::CheckCollision(colliderCup2, colliderSceneWall);
	CollisionPtr collisionBetweenCups = Collision::CheckCollision(colliderCup1, colliderCup2);
	CollisionPtr collisionCup1WithRay, collisionCup2WithRay;


	myEntity1->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir1, 1);
	myEntity1->Update();
	myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2, 1);
	myEntity2->Update();
	myEntity3->Update();


	if (mouse.rightButton)
	{
		XMFLOAT3 posOnGround = Raycast::GetPointOnGround(camera);
		//myEntity4->GetTransform()->SetPosition(posOnGround / myEntity4->GetTransform()->GetScale());
		myEntity4->GetTransform()->SetPosition(posOnGround);
	}

	myEntity4->Update();

	if (mouse.middleButton)
	{
		XMFLOAT3 cameraPos = camera.GetPositionFloat3();
		XMVECTOR origin = Vector4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
		XMFLOAT3 dirFromMouse = Raycast::GetRayDirFromMousePos(camera);
		XMVECTOR direction = Vector4(dirFromMouse.x, dirFromMouse.y, dirFromMouse.z, 0.0f);
		shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(origin, direction));
		collisionCup1WithRay = Collision::CheckCollision(colliderCup1, sharedRay);
		collisionCup2WithRay = Collision::CheckCollision(colliderCup2, sharedRay);
	}

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

	lightSystem->Iterate();

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

	renderableSystem->Iterate();

	//myEntity1->Model->Draw(context, *m_states, myEntity1->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	//myEntity2->Model->Draw(context, *m_states, myEntity2->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	//myEntity3->Model->Draw(context, *m_states, myEntity3->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	//myEntity4->Model->Draw(context, *m_states, myEntity4->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());


	m_boundingEntity1->Draw(boundingMatrix1, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider1Color, nullptr, true);
	m_boundingEntity2->Draw(boundingMatrix2, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider2Color, nullptr, true);

	// skinned model
	mSkinModel->DrawModel(context, *m_states, mSkinModelTransform->GetTransformMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	
	// billboarding
	//m_plane->Draw(planeWorld, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
	m_plane2->Draw(planeWorld2, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
	m_plane3->Draw(planeWorld3, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
	m_plane4->Draw(planeWorld4, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_planeTex.Get());
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
	

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(XMConvertToRadians(70.f), float(size.right) / float(size.bottom), 0.01f, 100.f);
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	camera.SetZoom(XMFLOAT3(0.f, 0.f, 0.f));
}

void Game::InitializeObjects(ID3D11Device1 *device, ID3D11DeviceContext1 *context)
{
	m_world = Matrix::Identity;

	entityManager = std::make_shared<EntityManager>();

	componentFactory = std::make_shared<ComponentFactory>(entityManager);

	renderableSystem = std::make_shared<RenderableSystem>(entityManager, device, context);

	lightSystem = std::make_shared<LightSystem>(entityManager, renderableSystem->_fxFactory);


	sceneWallEntity = entityManager->GetEntity(entityManager->CreateEntity("SceneWall"));
	myEntity1 = entityManager->GetEntity(entityManager->CreateEntity("Cup1"));
	myEntity2 = entityManager->GetEntity(entityManager->CreateEntity("Cup2"));
	myEntity3 = entityManager->GetEntity(entityManager->CreateEntity("Cup3"));
	myEntity4 = entityManager->GetEntity(entityManager->CreateEntity("Cup4"));

	std::shared_ptr<RenderableComponent> renderableComponent = std::make_shared<RenderableComponent>(L"cup.cmo", camera);
	std::shared_ptr<RenderableComponent> renderableComponent2 = std::make_shared<RenderableComponent>(L"cup.cmo", camera);
	std::shared_ptr<RenderableComponent> renderableComponent3 = std::make_shared<RenderableComponent>(L"cup.cmo", camera);
	std::shared_ptr<RenderableComponent> renderableComponent4 = std::make_shared<RenderableComponent>(L"cup.cmo", camera);

	componentFactory->CreateComponent(myEntity1, renderableComponent);
	componentFactory->CreateComponent(myEntity2, renderableComponent2);
	componentFactory->CreateComponent(myEntity3, renderableComponent3);
	componentFactory->CreateComponent(myEntity4, renderableComponent4);


	Vector3 scaleEntity1(0.5f, 0.5f, 0.5f), scaleEntity2(0.2f, 0.2f, 0.2f), scaleEntity3(0.3f, 0.3f, 0.3f), scaleEntity4(0.35f, 0.35f, 0.35f);

	renderableSystem->Initialize();

	// pointLightEntity1 - RED light follow player
	pointLightEntity1 = entityManager->GetEntity(entityManager->CreateEntity("PointLight1"));
	pointLightEntity2 = entityManager->GetEntity(entityManager->CreateEntity("PointLight2"));
	pointLightEntity3 = entityManager->GetEntity(entityManager->CreateEntity("PointLight3"));
	spotLightEntity1 = entityManager->GetEntity(entityManager->CreateEntity("SpotLight1"));

	pointLightEntity1->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	pointLightEntity2->GetTransform()->SetPosition(Vector3(-2.0f, 0.0f, 2.0f));
	pointLightEntity3->GetTransform()->SetPosition(Vector3(2.0f, -1.0f, 1.0f));
	spotLightEntity1->GetTransform()->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	// lightComponent - RED light follow player
	std::shared_ptr<LightComponent> lightComponent = std::make_shared<LightComponent>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), pointLightEntity1->GetTransform()->GetPosition(), 3.0f, true);
	std::shared_ptr<LightComponent> lightComponent2 = std::make_shared<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity2->GetTransform()->GetPosition(), 3.0f);
	std::shared_ptr<LightComponent> lightComponent3 = std::make_shared<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity3->GetTransform()->GetPosition(), 3.0f);
	std::shared_ptr<LightComponent> lightComponent4 = std::make_shared<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), 0.25f, spotLightEntity1->GetTransform()->GetPosition(), 0.75f, 10.0f);

	componentFactory->CreateComponent(pointLightEntity1, lightComponent);
	componentFactory->CreateComponent(pointLightEntity2, lightComponent2);
	componentFactory->CreateComponent(pointLightEntity3, lightComponent3);
	componentFactory->CreateComponent(spotLightEntity1, lightComponent4);

	lightSystem->Initialize();


	//myEntity1->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity1->GetTransform()->SetScale(scaleEntity1);
	myEntity1->GetTransform()->SetPosition(Vector3(-1.0f, 0.0f, 0.0f));

	//myEntity2->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));

	//myEntity3->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity3->GetTransform()->SetScale(scaleEntity3);
	myEntity3->GetTransform()->SetPosition(Vector3(0.0f, -1.5f, 0.0f));

	//myEntity4->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_ToonFactory);
	myEntity4->GetTransform()->SetScale(scaleEntity4);
	myEntity4->GetTransform()->SetPosition(Vector3(0.0f, -1.0f, -3.0f));

	myEntity1->AddChild(myEntity3);

	initialBoundingEntity1Size = XMFLOAT3(0.3f, 0.3f, 0.3f);
	initialBoundingEntity2Size = XMFLOAT3(0.8f, 0.8f, 0.8f);
	initialBounding1Radius = 0.3f;
	initialBounding2Radius = 0.8f;

	collisionSystem = std::make_shared<PhysicsSystem>(entityManager, Vector3::Zero, ROOM_BOUNDS[0]);

	colliderSceneWall = std::make_shared<PhysicsComponent>(AABB);
	colliderSceneWall->SetParent(sceneWallEntity);
	colliderBoundingSceneWall = std::dynamic_pointer_cast<ColliderAABB>(colliderSceneWall->ColliderBounding);
	colliderBoundingSceneWall->Bounding.Extents = XMFLOAT3(ROOM_BOUNDS[0] / 2.0f, ROOM_BOUNDS[1] / 2.0f, ROOM_BOUNDS[2] / 2.0f);
	//collisionSystem->InsertComponent(colliderSceneWall);

	colliderCup1 = std::make_shared<PhysicsComponent>(AABB);
	colliderCup1->SetParent(myEntity1);
	colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup1->ColliderBounding);
	colliderBoundingCup1->Bounding.Extents = initialBoundingEntity1Size;
	collisionSystem->InsertComponent(colliderCup1);

	colliderCup2 = std::make_shared<PhysicsComponent>(Sphere);
	colliderCup2->SetParent(myEntity2);
	colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup2->ColliderBounding);
	colliderBoundingCup2->Bounding.Radius = initialBounding2Radius;
	collisionSystem->InsertComponent(colliderCup2);

	collisionSystem->Initialize();

	m_room = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
		false, true);

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
	mSkinModelTransform->SetPosition(Vector3(1.0f, -3.0f, 0.0f));

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
	audioSystem = std::make_shared<AudioSystem>(entityManager);
	audioSystem->Initialize();
	audioBackgroundSound = std::dynamic_pointer_cast<AudioComponent>(entityManager->GetEntityComponentsOfType(entityManager->GetEntity("BackgroundAudioEntity")->GetId(), ComponentType("Audio"))[0]);
	audioSound1 = std::dynamic_pointer_cast<AudioComponent>(entityManager->GetEntityComponentsOfType(entityManager->GetEntity("Sound1AudioEntity")->GetId(), ComponentType("Audio"))[0]);
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