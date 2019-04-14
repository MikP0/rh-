//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//To DELETE
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include "Transform.h"
//

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

	std::vector<actionList> pushedKeysActions = inputSystem->GetActions(inputComponent);
	inputSystem->SetMouseMode(mouse.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);

	for (std::vector<actionList>::iterator iter = pushedKeysActions.begin(); iter != pushedKeysActions.end(); ++iter)
	{
		// zmiana MouseMode tutaj z udzia³em InputSystemu spowalnia renderowanie przy obracaniu (nie wiem czemu)
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

	/*g_BatchEffect->SetWorld(m_world);
	g_BatchEffect->SetView(camera.GetViewMatrix());
	g_BatchEffect->SetProjection(camera.GetProjectionMatrix());*/

	// check collisions
	static Vector3 dir1(1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	static bool coolidedBefore = false;
	collisionSystem->UpdateColliders();
	CollisionPtr collision = collisionSystem->Collide(colliderCup1, colliderCup2);
	
	if (collision != nullptr && !coolidedBefore)
	{
			coolidedBefore = true;
			dir1 *= -1.0f;
			dir2 *= -1.0f;
	}

	// room
	m_room->Draw(Matrix::Identity, camera.GetViewMatrix(), camera.GetProjectionMatrix(), Colors::White, m_roomTex.Get());

	// cup
	myEntity1->Model->Draw(context, *m_states, myEntity1->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	myEntity2->Model->Draw(context, *m_states, myEntity2->GetWorldMatrix(), camera.GetViewMatrix(), camera.GetProjectionMatrix());
	
	
	/*myEntity2->GetTransform()->SetPosition(Vector3(0.2f, 0.0f, 1.5f));

	myEntity1->GetTransform()->Translate(Vector3(0.0f, 0.1f, 0.0f));
	myEntity1->GetTransform()->SetScale(Vector3(0.2, 0.2, 0.2));
	myEntity2->GetTransform()->SetScale(Vector3(1.2, 1.2, 1.2));
	myEntity1->Update();*/

	myEntity1->GetTransform()->Translate(Vector3(-0.1f, 0.0f, 0.0f) * dir1);
	myEntity2->GetTransform()->Translate(Vector3(0.1f, 0.0f, 0.0f) * dir2);
	myEntity1->GetTransform()->SetScale(Vector3(0.5, 0.5, 0.5));
	myEntity2->GetTransform()->SetScale(Vector3(0.5, 0.5, 0.5));
	myEntity1->Update();
	myEntity2->Update();
	
	if (myEntity1->GetTransform()->GetPosition().x >= (ROOM_BOUNDS[0] - 0.5f)
		|| myEntity1->GetTransform()->GetPosition().x <= (-ROOM_BOUNDS[0] + 0.5f))
	{
		coolidedBefore = false;
		dir1 *= -1.0f;
	}

	if (myEntity2->GetTransform()->GetPosition().x >= (ROOM_BOUNDS[0] - 0.5f)
		|| myEntity2->GetTransform()->GetPosition().x <= (-ROOM_BOUNDS[0] + 0.5f))
	{
		coolidedBefore = false;
		dir2 *= -1.0f;
	}

	/*if (collision != nullptr)
	{
		ExitGame();
	}

	XMVECTOR c1 = Collision::GetCollisionColor(colliderCup1->CollisionBox.CollisionKind);
	DrawAabb(colliderCup1->CollisionBox.BoundingBox, c1);

	XMVECTOR c2 = Collision::GetCollisionColor(colliderCup2->CollisionBox.CollisionKind);
	DrawAabb(colliderCup2->CollisionBox.BoundingBox, c2);*/


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
	width = 800;
	height = 600;
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
	myEntity1->GetTransform()->SetPosition(Vector3(-1.0f, 0.0f, 0.0f));

	myEntity2->Model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	myEntity2->SetWorldMatrix(m_world);
	myEntity2->GetTransform()->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	//myEntity1->AddChild(myEntity2);

	colliderCup1 = std::make_shared<PhysicsComponent>();
	colliderCup1->SetParent(myEntity1);
	colliderCup2 = std::make_shared<PhysicsComponent>();
	colliderCup2->SetParent(myEntity2);
	colliderCup1->CollisionBox.BoundingBox.Extents = XMFLOAT3(0.4f, 0.4f, 0.4f);
	colliderCup1->CollisionBox.BoundingBox.Extents = XMFLOAT3(0.4f, 0.4f, 0.4f);
	collisionSystem = std::make_shared<PhysicsSystem>();
	collisionSystem->InsertComponent(colliderCup1);
	collisionSystem->InsertComponent(colliderCup2);
	

	m_room = GeometricPrimitive::CreateBox(context,
		XMFLOAT3(ROOM_BOUNDS[0], ROOM_BOUNDS[1], ROOM_BOUNDS[2]),
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
	m_roomTex.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion

//--------------------------------------------------------------------------------------
void Game::DrawCube(CXMMATRIX mWorld, FXMVECTOR color)
{
	static const XMVECTOR s_verts[8] =
	{
		{ -1, -1, -1, 0 },
		{ 1, -1, -1, 0 },
		{ 1, -1, 1, 0 },
		{ -1, -1, 1, 0 },
		{ -1, 1, -1, 0 },
		{ 1, 1, -1, 0 },
		{ 1, 1, 1, 0 },
		{ -1, 1, 1, 0 }
	};
	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	VertexPositionColor verts[8];
	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR v = XMVector3Transform(s_verts[i], mWorld);
		XMStoreFloat3(&verts[i].position, v);
		XMStoreFloat4(&verts[i].color, color);
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	g_BatchEffect->Apply(context);

	//context->IASetInputLayout(g_pBatchInputLayout.get());

	g_Batch->Begin();

	g_Batch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), verts, 8);

	g_Batch->End();
}


//--------------------------------------------------------------------------------------
void Game::DrawAabb(const BoundingBox& box, FXMVECTOR color)
{
	XMMATRIX matWorld = XMMatrixScaling(box.Extents.x, box.Extents.y, box.Extents.z);
	XMVECTOR position = XMLoadFloat3(&box.Center);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, color);
}

void Game::DrawRay(FXMVECTOR Origin, FXMVECTOR Direction, bool bNormalize, FXMVECTOR color)
{
	VertexPositionColor verts[3];
	XMStoreFloat3(&verts[0].position, Origin);

	XMVECTOR NormDirection = XMVector3Normalize(Direction);
	XMVECTOR RayDirection = (bNormalize) ? NormDirection : Direction;

	XMVECTOR PerpVector = XMVector3Cross(NormDirection, g_XMIdentityR1);

	if (XMVector3Equal(XMVector3LengthSq(PerpVector), g_XMZero))
	{
		PerpVector = XMVector3Cross(NormDirection, g_XMIdentityR2);
	}
	PerpVector = XMVector3Normalize(PerpVector);

	XMStoreFloat3(&verts[1].position, XMVectorAdd(RayDirection, Origin));
	PerpVector = XMVectorScale(PerpVector, 0.0625f);
	NormDirection = XMVectorScale(NormDirection, -0.25f);
	RayDirection = XMVectorAdd(PerpVector, RayDirection);
	RayDirection = XMVectorAdd(NormDirection, RayDirection);
	XMStoreFloat3(&verts[2].position, XMVectorAdd(RayDirection, Origin));

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);

	auto context = m_deviceResources->GetD3DDeviceContext();
	g_BatchEffect->Apply(context);

	//context->IASetInputLayout(g_pBatchInputLayout);

	g_Batch->Begin();

	g_Batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);

	g_Batch->End();
}


/*std::string ma = std::to_string(m_world._11) + "\t" + std::to_string(m_world._12) + "\t" + std::to_string(m_world._13) + "\t";
	ma = ma + "\n";
	ma = ma + std::to_string(m_world._21) + "\t" + std::to_string(m_world._22) + "\t" + std::to_string(m_world._23) + "\t";
	ma = ma + "\n";
	ma = ma + std::to_string(m_world._31) + "\t" + std::to_string(m_world._32) + "\t" + std::to_string(m_world._33) + "\t";
	char text[250];
	strcpy(text, ma.c_str());
	OutputDebugStringA(text);*/