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
	inputComponent = std::make_shared<InputComponent>(actionKeysBindings);
	inputComponent->SetParent(inputEntity);
	inputSystem = std::make_shared<InputSystem>();
	inputSystem->InsertComponent(inputComponent);
	inputSystem->SetWindowForMouse(window);

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
	auto mouse = inputSystem->GetMouseState();
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

	std::vector<actionList> pushedKeysActions = inputSystem->GetActions(inputComponent);
	inputSystem->SetMouseMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter)
	{
		// zmiana MouseMode tutaj z udzia�em InputSystemu spowalnia renderowanie przy obracaniu (nie wiem czemu)
		//inputSystem->SetMouseMode(*iter == anchorRotation ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

		if (*iter == closeWindow)
			ExitGame();

		if (*iter == up)
			move.y += 1.f;

		if (*iter == down)
			move.y -= 1.f;

		if (*iter == left)
			move.x += 1.f;

		if (*iter == right)
			move.x -= 1.f;

		if (*iter == forward)
			move.z += 1.f;

		if (*iter == backward)
			move.z -= 1.f; 

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

	elapsedTime;
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

	// check collisions
	static Vector3 dir1(1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	XMVECTORF32 collider1Color = DirectX::Colors::White;
	XMVECTORF32 collider2Color = DirectX::Colors::White;
	static bool coolidedBefore = false;
	collisionSystem->UpdateColliders();
	CollisionPtr collision = collisionSystem->Collide(colliderCup1, colliderCup2);
	
	if (collision != nullptr && !coolidedBefore)
	{
			coolidedBefore = true;
			dir1 *= -1.0f;
			dir2 *= -1.0f;
			collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding.CollisionKind);
			collider2Color = Collision::GetCollisionColor(colliderCup2->ColliderBounding.CollisionKind);
	}

	// cup
	dxmath::Matrix transformedMatrix1 = colliderCup1->GetParent()->GetWorldTransformMatrix();
	dxmath::Matrix transformedMatrix2 = colliderCup2->GetParent()->GetWorldTransformMatrix();

	Vector3 scaleEntity1(0.5, 0.5, 0.5), scaleEntity2(0.5, 0.5, 0.5);

	myEntity1->GetTransform()->SetScale(scaleEntity1);
	colliderCup1->ColliderBounding.Bounding.Extents = Vector3(initialBoundingEntity1Size) * Vector3(scaleEntity1 / 5.0f);
	myEntity1->GetTransform()->Translate(Vector3(-0.05f, 0.0f, 0.0f) * dir1);
	myEntity1->Update();
	colliderCup2->ColliderBounding.Bounding.Extents = Vector3(initialBoundingEntity2Size) * Vector3(scaleEntity2 / 5.0f);
	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2);
	myEntity2->Update();
	
	if (myEntity1->GetTransform()->GetPosition().x > (ROOM_BOUNDS[0]/2.0f)
		|| myEntity1->GetTransform()->GetPosition().x < (-ROOM_BOUNDS[0]/2.0f))
	{
		coolidedBefore = false;
		dir1 *= -1.0f;
		collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding.CollisionKind);
	}

	if (myEntity2->GetTransform()->GetPosition().x >= (ROOM_BOUNDS[0]/2.0f)
		|| myEntity2->GetTransform()->GetPosition().x <= (-ROOM_BOUNDS[0]/ 2.0f))
	{
		coolidedBefore = false;
		dir2 *= -1.0f;
		collider2Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding.CollisionKind);
	}

	// room
	m_room->Draw(m_world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_roomTex.Get());
	//m_room->Draw(m_world, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::DeepPink, nullptr, false);
	
	// cups
	myEntity1->Model->Draw(context, *m_states, transformedMatrix1, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	myEntity2->Model->Draw(context, *m_states, transformedMatrix2, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	m_boundingEntity1->Draw(transformedMatrix1, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider1Color, nullptr, true);
	m_boundingEntity2->Draw(transformedMatrix2, camera.GetViewMatrix(), camera.GetProjectionMatrix(), collider2Color, nullptr, true);



	context;

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
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

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1280;
	height = 800;
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
	//g_Batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//g_BatchEffect = std::make_unique<BasicEffect>(device);
	//g_BatchEffect->SetVertexColorEnabled(true);

	//{
	//	void const* shaderByteCode;
	//	size_t byteCodeLength;

	//	g_BatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	//	/*hr = device->CreateInputLayout(VertexPositionColor::InputElements,
	//		VertexPositionColor::InputElementCount,
	//		shaderByteCode, byteCodeLength,
	//		&g_pBatchInputLayout);
	//	if (FAILED(hr))
	//		return hr;*/
	//}

	m_fxFactory = std::make_unique<EffectFactory>(device);

	m_world = Matrix::Identity;

	myEntity1 = std::make_shared<Entity>();
	myEntity2 = std::make_shared<Entity>();

	myEntity1->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity1->SetWorldMatrix(m_world);
	myEntity1->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	myEntity2->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity2->SetWorldMatrix(m_world);
	myEntity2->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	//myEntity1->AddChild(myEntity2);

	initialBoundingEntity1Size = XMFLOAT3(1.0f, 1.0f, 1.0f);
	initialBoundingEntity2Size = XMFLOAT3(1.0f, 1.0f, 1.0f);

	collisionSystem = std::make_shared<PhysicsSystem>();
	colliderCup1 = std::make_shared<PhysicsComponent<ColliderAABB>>();
	colliderCup1->SetParent(myEntity1);
	colliderCup1->ColliderBounding.Bounding.Extents = initialBoundingEntity1Size;
	collisionSystem->InsertComponent(colliderCup1);
	colliderCup2 = std::make_shared<PhysicsComponent<ColliderAABB>>();
	colliderCup2->SetParent(myEntity2);
	colliderCup2->ColliderBounding.Bounding.Extents = initialBoundingEntity2Size;
	collisionSystem->InsertComponent(colliderCup2);
	

	m_room = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
		false, true);

	m_boundingEntity1 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(colliderCup1->ColliderBounding.Bounding.Extents.x, 
			colliderCup1->ColliderBounding.Bounding.Extents.y, 
			colliderCup1->ColliderBounding.Bounding.Extents.z),
		false, true);

	m_boundingEntity2 = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(colliderCup2->ColliderBounding.Bounding.Extents.x,
			colliderCup2->ColliderBounding.Bounding.Extents.y,
			colliderCup2->ColliderBounding.Bounding.Extents.z),
		false, true);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"roomtexture.dds",
			nullptr, m_roomTex.ReleaseAndGetAddressOf()));

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

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_fxFactory.reset();

	myEntity1->Model.reset();
	myEntity2->Model.reset();

	m_room.reset();
	m_boundingEntity1.reset();
	m_boundingEntity2.reset();
	m_roomTex.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion


/*std::string ma = std::to_string(m_world._11) + "\t" + std::to_string(m_world._12) + "\t" + std::to_string(m_world._13) + "\t";
	ma = ma + "\n";
	ma = ma + std::to_string(m_world._21) + "\t" + std::to_string(m_world._22) + "\t" + std::to_string(m_world._23) + "\t";
	ma = ma + "\n";
	ma = ma + std::to_string(m_world._31) + "\t" + std::to_string(m_world._32) + "\t" + std::to_string(m_world._33) + "\t";
	char text[250];
	strcpy(text, ma.c_str());
	OutputDebugStringA(text);*/