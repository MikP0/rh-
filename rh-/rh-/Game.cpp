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
#include "DebugDraw.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace
{
	const XMVECTORF32 ROOM_BOUNDS = { 1.f, 0.f, 1.f, 0.f }; //REMOVE
	const float COLLISION_SCENE_RANGE = 55.0f; // Octtree construcor
	const Vector3 SCENE_CENTER = Vector3(20, 0, 20); //Octtree constructor
	const float ROTATION_GAIN = 0.008f; // REMOVE
	const float MOVEMENT_GAIN = 0.07f; // REMOVE

	const XMVECTORF32 PLANE_BOUNDS = { 1.5f, 1.5f, 1.5f, 0.f }; //REMOVE
}

bool initTerrain = false;

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

	total_Time = time;
	elapsed_Time = elapsedTime;

	Coroutine::UpdateElapsedTime(elapsedTime);

	// TODO: Add your game logic here.
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

	// TO DISABLE MENU and START SCREENS
	//mainMenu = false;

	if (gameStage < 5)
	{
		SkipStartScreen();

		startTimer += elapsedTime;

		if (mainMenu)
		{
			if (startTimer > 4.0f) // Rh- screen
			{
				if (startTimer > 8.0f)	// Wicked screen
				{
					if (startTimer > 12.0f)	// DirectX screen
					{
						if (startTimer > 13.0f)	// mainMenu Screen
						{
							if (gameStage == 4)	// loading Screen
							{
								gameStage = 5;
								InitializeAll(device, context);
								plotStage = 1;					
							}
							else
								UpdateMainMenu(elapsedTime);
						}
						else
							gameStage = 3;
					}
					else
						gameStage = 2;
				}
				else
					gameStage = 1;
			}
		}
		else
		{
			plotScreens = false;
			gameStage = 6;
			InitializeAll(device, context);
		}
	}
	else if (gameStage == 5)
	{
		SkipPlot();

		if (plotScreens)
		{
			plotTimer += elapsedTime;
			ColorChanger += elapsedTime / 3.0f;

			if (plotTimer > 3.0f)
			{
				if (plotTimer > 6.0f)
				{
					if (plotTimer > 9.0f)
					{
						if (plotTimer > 12.0f)
						{
							if (plotTimer > 15.0f)
							{
								if (plotTimer > 18.0f)
								{
									if (plotTimer > 21.0f)
									{
										if (plotTimer > 24.0f)
										{
											if (plotTimer > 27.0f)
											{
												if (plotTimer > 30.0f)
												{
													gameStage = 6;
												}
												else
													plotStage = 10;
											}
											else
												plotStage = 9;
										}
										else
											plotStage = 8;
									}
									else
										plotStage = 7;
								}
								else
									plotStage = 6;
							}
							else
								plotStage = 5;
						}
						else
							plotStage = 4;
					}
					else
						plotStage = 3;
				}
				else
					plotStage = 2;
			}
		}
		else
		{
			gameStage = 6;
		}
	}
	else if (gameStage == 6)
	{
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

				/*if (!vampireMode)
				{
					if (*iter == special1)
					{
						//for (auto component : world->GetComponents<RenderableComponent>())
						//{
						//	if (strcmp(component->GetParent()->GetName().c_str(),
						//		"Player") == 0)
						//	{
						//		component->_modelSkinned->GetAnimatorPlayer()->StartClip("HipHop"); // -> Player Entity -> Player Component
						//		component->_modelSkinned->SetInMove(true);
						//		component->_modelSkinned->GetAnimatorPlayer()->SetDirection(true);
						//		isDancing = true;
						//	}
						//}
					}

					if (*iter == special2)
					{
						//for (auto component : world->GetComponents<RenderableComponent>())
						//{
						//	if (strcmp(component->GetParent()->GetName().c_str(),
						//		"Player") == 0)
						//	{
						//		component->_modelSkinned->GetAnimatorPlayer()->StartClip("Dance"); // -> Player Entity -> Player Component
						//		component->_modelSkinned->SetInMove(true);
						//		component->_modelSkinned->GetAnimatorPlayer()->SetDirection(true);
						//		isDancing = true;
						//	}
						//}
					}
				}*/

				if (*iter == playBackground)
				{
					//world->ClearWorld();

					//auto device = m_deviceResources->GetD3DDevice();
					//auto context = m_deviceResources->GetD3DDeviceContext();


					//collisionSystem = std::make_shared<PhysicsSystem>(SCENE_CENTER, COLLISION_SCENE_RANGE, camera);
					//renderableSystem = std::make_shared<RenderableSystem>(device, context, collisionSystem);
					////renderableSystem = std::make_shared<RenderableSystem>(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
					////lightSystem = std::make_shared<LightSystem>(renderableSystem->_fxFactory);

					//// Adding systems to world ------------------------------------------------------------------
					//world->AddSystem<PhysicsSystem>(collisionSystem, 0);
					////world->AddSystem<LightSystem>(lightSystem, 1);
					////world->AddSystem<AudioSystem>(audioSystem, 2);
					//world->AddSystem<RenderableSystem>(renderableSystem, 3);

					//playerEntity = world->CreateEntity("Player");


					//worldLoader->LoadWorldFromXML("testLevel.xml");

					////world->InitializeSystem<AudioSystem>();
					//world->InitializeSystem<PhysicsSystem>();
					////world->InitializeSystem<LightSystem>();
					//world->InitializeSystem<RenderableSystem>();


					//playerEntity->AddComponent<RenderableComponent>(L"content\\Models\\Hero.fbx", &camera);

					audioBackgroundSound->Mute = false;
				}

				//if (*iter == playSound1)
				//{
				//	audioSound1->Mute = false;

					//if ((healthBarHealthPos.x <= 135.0f) && (healthBarHealthPos.x >= -150.0f))
					//	healthBarHealthPos.x -= 5.f;
				//}

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
			//Camera Movement
			if (freeCameraLook)
			{
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

			else
			{
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
		Ui->fpsFontText = std::wstring(str.begin(), str.end());

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

	}

	elapsedTime;
}

void Game::UpdateMainMenu(float elapsedTime)
{
	auto mouse = Input::GetMouseState();
	auto keyboard = Input::GetKeyboardState();
	auto size = m_deviceResources->GetOutputSize();

	tracker.Update(mouse);
	keyboardTracker.Update(keyboard);

	if (tracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		if ((mouse.x >= size.left + 0.15f * size.right) && (mouse.x <= size.left + 0.33f * size.right))
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////START
			if ((mouse.y >= size.top + 0.11f * size.bottom) && (mouse.y <= size.top + 0.21f * size.bottom))			
			{
				gameStage = 4;
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////OPTIONS
			//else if ((mouse.y >= size.top + 0.42f * size.bottom) && (mouse.y <= size.top + 0.53f * size.bottom))
			//{
			//	// DO NOTHING
			//}

			/////////////////////////////////////////////////////////////////////////////////////////////////EXIT
			else if ((mouse.y >= size.top + 0.7f * size.bottom) && (mouse.y <= size.top + 0.81f * size.bottom))
			{
				ExitGame();
			}
		}
	}
}

void Game::UpdateObjects(float elapsedTime)
{
	auto mouse = Input::GetMouseState();
	auto keyboard = Input::GetKeyboardState();

	tracker.Update(mouse);
	keyboardTracker.Update(keyboard);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (vampireMode)
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::Space))
		{
			vampireMode = false;
			playerSystem->SetVampireMode(false);
			enemySystem->SetVampireMode(false);
		}
	}
	else
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::Space))
		{
			vampireMode = true;
			Ui->transitionMode = true;
			playerSystem->SetVampireMode(true);
			enemySystem->SetVampireMode(true);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////




	//// check collisions
	//vector<CollisionPtr> currentCollisions = collisionSystem->AllCollisions;
	//vector<CollisionPtr> collisionsForEntity1 = collisionSystem->GetCollisionsForEntity(1);

	//static Vector3 dir1(-1.0f, 0.0f, 0.0f), dir2(1.0f, 0.0f, 0.0f);
	//XMVECTORF32 collider1Color = DirectX::Colors::White;
	//XMVECTORF32 collider2Color = DirectX::Colors::White;

	//CollisionPtr collisionCup1WithRay, collisionCup2WithRay;

	////myEntity1->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir1, 1);
	//myEntity2->GetTransform()->Translate(Vector3(0.05f, 0.0f, 0.0f) * dir2, 1);

	///*if (mouse.rightButton)
	//{
	//	XMFLOAT3 posOnGround = Raycast::GetPointOnGround(camera);
	//	myEntity4->GetTransform()->SetPosition(Vector3(posOnGround.x, 0.47f, posOnGround.z));
	//}*/

	//BoundingBox octrTreeBounding = collisionSystem->GetOctTree()->Region->GetBounding();


	///*if (octrTreeBounding.Contains(colliderBoundingCup1->GetBounding()) != CONTAINS)
	//{
	//	if (colliderBoundingCup1->GetCenter().x >= 0.0f)
	//		dir1.x = -1.0f;

	//	if (colliderBoundingCup1->GetCenter().x <= 0.0f)
	//		dir1.x = 1.0f;
	//}

	//if (octrTreeBounding.Contains(colliderBoundingCup2->GetBounding()) != CONTAINS)
	//{
	//	if (colliderBoundingCup2->GetCenter().x >= 0.0f)
	//		dir2.x = -1.0f;

	//	if (colliderBoundingCup2->GetCenter().x <= 0.0f)
	//		dir2.x = 1.0f;
	//}*/

	//if (keyboard.M)
	//{
	//	shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(camera));
	//	vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

	//	for each (shared_ptr<Collision> coll in collisionsWithRay)
	//	{
	//		if (coll->OriginObject->GetId() == colliderCup1->GetParent()->GetId())
	//			collisionCup1WithRay = coll;

	//		/*if (coll->OriginObject->GetId() == colliderCup2->GetParent()->GetId())
	//			collisionCup2WithRay = coll;*/
	//	}
	//}
}

#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	auto size = m_deviceResources->GetOutputSize();

	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	switch (gameStage)
	{
		case 0:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(startScreenTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 1:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(wickedScreenTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 2:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(directXTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 3:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(mainMenuTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 4:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(loadingScreenTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 5:
		{
			ShowPlot(plotStage);
			break;
		}
		case 6:
		{
			renderableSystem->SentResources(m_deviceResources->GetRenderTargetView(), m_deviceResources->GetDepthStencilView(), playerEntity, size.right, size.bottom, vampireMode);

			float vampireModeBrightness = brightness + 20.0f;

			if (vampireMode)
			{
				renderableSystem->BloomBlurParams.size = 25.0f;
			}
			else
			{
				renderableSystem->BloomBlurParams.size = 1.0f;
			}

			if (Input::GetKeyboardState().D7 && brightness > 1.0f)
			{
				brightness -= 0.2f;
			}

			if (Input::GetKeyboardState().D8 && brightness <= 7.0f)
			{
				brightness += 0.2f;
			}

			if (!vampireMode)
			{
				renderableSystem->BloomBlurParams.size = 1.0f;
				renderableSystem->BloomBlurParams.brightness = brightness;
			}
			else
			{
				renderableSystem->BloomBlurParams.size = 120.0f;
				renderableSystem->BloomBlurParams.brightness = vampireModeBrightness;
			}

			world->RefreshWorld();
			if (!initTerrain) {
				terrain->CreateWorld(world->GetComponents<PhysicsComponent>());
				//terrain->SetStaticObjects(world->GetComponents<PhysicsComponent>());
				initTerrain = true;
			}
			RenderObjects(context);
			break;
		}
		case 15:
		{
			m_spriteBatch->Begin();

			m_spriteBatch->Draw(mainMenuTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);
			
			m_spriteBatch->Draw(cattyTexture.Get(), Vector2(500.0f, 500.0f), nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		default:
			break;
	}



	context;

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::RenderObjects(ID3D11DeviceContext1 * context)
{
	DirectX::XMMATRIX cameraView = camera.GetViewMatrix(); // ?? FIXME
	DirectX::XMMATRIX cameraProjection = camera.GetProjectionMatrix();

	//XMVECTORF32 collider1Color = Collision::GetCollisionColor(colliderCup1->ColliderBounding->CollisionKind);
	//XMVECTORF32 collider2Color = Collision::GetCollisionColor(colliderCup2->ColliderBounding->CollisionKind);
	//terrain->Update(collisionSystem->GetColliders());
	/*if (vampireMode)
	{
		terrain->Draw(camera, m_roomTex);
	}*/
	if (debugDraw) //REMOVE
		renderableSystem->DebugDrawAction->DrawOctTree(
			collisionSystem->GetOctTree(), cameraView, cameraProjection, debugDrawTreeRegions);

	// TODO: UI 
	Ui->Draw(vampireMode, playerSystem->player->vampireAbility, menuIsOn, total_Time, elapsed_Time);
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
	int w = 1920, h = 1080;
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


	//m_states = std::make_shared<CommonStates>(device); //REMOVE

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
	camera.SetProjectionValues(XMConvertToRadians(70.f), float(size.right) / float(size.bottom), 0.1f, 30.f);
	camera.SetPitch(m_pitch);
	camera.SetYaw(m_yaw);
	camera.SetZoom(XMFLOAT3(0.f, 0.f, 0.f));
}

void Game::InitializeObjects(ID3D11Device1 * device, ID3D11DeviceContext1 * context)
{
	
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	m_ActualspriteBatch = std::make_unique<SpriteBatch>(context);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"menuMiko.dds",
			nullptr,
			mainMenuTexture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"WickedScreen.dds",
			nullptr,
			wickedScreenTexture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"StartScreen.dds",
			nullptr,
			startScreenTexture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"DirectXScren.dds",
			nullptr,
			directXTexture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"LoadingScreen.dds",
			nullptr,
			loadingScreenTexture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot1.dds",
			nullptr,
			plot1Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot2.dds",
			nullptr,
			plot2Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot3.dds",
			nullptr,
			plot3Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot4.dds",
			nullptr,
			plot4Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot5.dds",
			nullptr,
			plot5Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot6.dds",
			nullptr,
			plot6Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot7.dds",
			nullptr,
			plot7Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot8.dds",
			nullptr,
			plot8Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot9.dds",
			nullptr,
			plot9Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Plot10.dds",
			nullptr,
			plot10Texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"BlackBackScreen.dds",
			nullptr,
			blackBackTexture.ReleaseAndGetAddressOf()));

	
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"cat.dds",
			nullptr,
			cattyTexture.ReleaseAndGetAddressOf()));

	states = std::make_unique<CommonStates>(device);


	auto size = m_deviceResources->GetOutputSize();
	m_screenPos.x = 0;
	m_screenPos.y = 0;
}

void Game::InitializeAll(ID3D11Device1 * device, ID3D11DeviceContext1 * context)
{
	cooldown = make_shared<Cooldown>(skillsNames, skillsTimeLimits);

	terrain = std::make_shared<Terrain>();
	world = std::make_shared<World>();
	worldLoader = std::make_shared<WorldLoader>(world, &camera);

	worldLoader->LoadWorldFromXML("testLevel.xml");
	worldLoader->LoadWorldFromXML("testLevel2.xml");

	// Creation of systems ------------------------------------------------------------------
	audioSystem = std::make_shared<AudioSystem>();
	collisionSystem = std::make_shared<PhysicsSystem>(SCENE_CENTER, COLLISION_SCENE_RANGE, camera);
	renderableSystem = std::make_shared<RenderableSystem>(device, context, collisionSystem);
	lightSystem = std::make_shared<LightSystem>(renderableSystem->_ShadowsfxFactory, renderableSystem->_noShadowsfxFactory);
	enemySystem = std::make_shared<EnemySystem>();
	playerSystem = std::make_shared<PlayerSystem>(collisionSystem, &camera);

	// Adding systems to world ------------------------------------------------------------------
	world->AddSystem<PhysicsSystem>(collisionSystem, 0);
	world->AddSystem<PlayerSystem>(playerSystem, 1);
	world->AddSystem<EnemySystem>(enemySystem, 2);
	world->AddSystem<AudioSystem>(audioSystem, 3);
	world->AddSystem<LightSystem>(lightSystem, 4);
	world->AddSystem<RenderableSystem>(renderableSystem, 5);

	/*
		FILL WORLD OBJECT
	*/

	// Creation of entities ------------------------------------------------------------------
	myEntity1 = world->CreateEntity("Cup1");
	myEntity2 = world->CreateEntity("Cup2");
	myEntity3 = world->CreateEntity("Cup3");
	myEntity4 = world->CreateEntity("Cup4");

	backgroundAudio = world->CreateEntity("BackgroundAudio");
	damageAudio = world->CreateEntity("DamageAudio");
	playerFootstepAudio = world->CreateEntity("PlayerFootstepAudio");
	enemyFootstepAudio = world->CreateEntity("EnemyFootstepAudio");
	swordSlashAudio = world->CreateEntity("SwordSlashAudio");

	pointLightEntity1 = world->CreateEntity("PointLight1");
	spotLightEntity1 = world->CreateEntity("SpotLight1");
	//directLightEntity1 = world->CreateEntity("DirectLight1");
	myEntityFloor = world->CreateEntity("FloorForShadows");
	playerEntity = world->CreateEntity("Player");
	enemyEntity1 = world->CreateEntity("Enemy1");
	enemyEntity2 = world->CreateEntity("Enemy2");
	enemyEntity3 = world->CreateEntity("Enemy3");
	enemyEntity4 = world->CreateEntity("Enemy4");
	enemyEntity5 = world->CreateEntity("Enemy5");
	enemyEntity6 = world->CreateEntity("Enemy6");


	// Creation of renderable components
	//myEntity1->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	//myEntity2->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	//myEntity3->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	//myEntity4->AddComponent<RenderableComponent>(L"cup.cmo", &camera);
	//myEntityFloor->AddComponent<RenderableComponent>(L"NFloor.cmo", &camera);
	playerEntity->AddComponent<RenderableComponent>(L"content\\Models\\Anna.fbx", &camera);
	enemyEntity1->AddComponent<RenderableComponent>(L"content\\Models\\Brute.fbx", &camera);
	enemyEntity2->AddComponent<RenderableComponent>(L"content\\Models\\Brute.fbx", &camera);
	enemyEntity3->AddComponent<RenderableComponent>(L"content\\Models\\Brute.fbx", &camera);
	enemyEntity4->AddComponent<RenderableComponent>(L"content\\Models\\Brute.fbx", &camera);
	enemyEntity5->AddComponent<RenderableComponent>(L"content\\Models\\Brute.fbx", &camera);
	enemyEntity6->AddComponent<RenderableComponent>(L"content\\Models\\EnemyGuard.fbx", &camera);

	// Creation of audio components ------------------------------------------------------------------
	backgroundAudio->AddComponent<AudioComponent>("Resources\\Audio\\background_music.wav");
	swordSlashAudio->AddComponent<AudioComponent>("Resources\\Audio\\attack.wav");
	damageAudio->AddComponent<AudioComponent>("Resources\\Audio\\KnifeSlice.wav");
	enemyFootstepAudio->AddComponent<AudioComponent>("Resources\\Audio\\temp.wav");
	playerFootstepAudio->AddComponent<AudioComponent>("Resources\\Audio\\playerStep.wav");
	// Creation of physics components ----------------------------------------------------------------
	myEntity1->AddComponent<PhysicsComponent>(Vector3::Zero, XMFLOAT3(.49f, 1.5f, 4.49f), false);
	//myEntity2->AddComponent<PhysicsComponent>(Vector3::Zero, 0.7f, false);
	enemyEntity1->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	enemyEntity2->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	enemyEntity3->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	enemyEntity4->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	enemyEntity5->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	enemyEntity6->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);
	playerEntity->AddComponent<PhysicsComponent>(Vector3(0, 80.0f, 0), XMFLOAT3(0.4f, 1.0f, 0.4f), true);

	// Creation of enemy components ------------------------------------------------------------------
	enemyEntity1->AddComponent<EnemyComponent>(3.f, 20);
	enemyEntity2->AddComponent<EnemyComponent>(3.f, 18);
	enemyEntity3->AddComponent<EnemyComponent>(3.f, 22);
	enemyEntity4->AddComponent<EnemyComponent>(3.f, 19);
	enemyEntity5->AddComponent<EnemyComponent>(3.f, 23);
	enemyEntity6->AddComponent<EnemyComponent>(10.f, 35, 1.9f, 3.0f);


	playerEntity->AddComponent<PlayerComponent>();


	pointLightEntity1->GetTransform()->SetPosition(Vector3(0.0f, 3.5f, 2.0f));

	// Creation of light components ------------------------------------------------------------------
	pointLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), pointLightEntity1->GetTransform()->GetPosition(), 108.0f);
	//spotLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), 0.25f, spotLightEntity1->GetTransform()->GetPosition(), 0.75f, 10.0f);
	//directLightEntity1->AddComponent<LightComponent>(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, -0.5f));
	// Setting up transform parameters of entities  --------------------------------------------------
	Vector3 scaleEntity1(0.1f, 0.1f, 0.1f), scaleEntity2(0.2f, 0.2f, 0.2f), scaleEntity3(0.3f, 0.3f, 0.3f), scaleEntity4(1.0f, 1.0f, 1.0f);
	myEntity1->GetTransform()->SetScale(scaleEntity1);
	myEntity1->GetTransform()->SetPosition(Vector3(-30.0f, 1.25f, 20.0f));

	myEntity2->GetTransform()->SetScale(scaleEntity2);
	myEntity2->GetTransform()->SetPosition(Vector3(6.0f, 0.2f, 6.0f));

	myEntity3->GetTransform()->SetPosition(Vector3(0.0f, -1.5f, 0.0f));

	/*myEntityFloor->GetTransform()->SetScale(Vector3(0.3f, 0.3f, 0.3f));
	myEntityFloor->GetTransform()->SetPosition(Vector3(0.f, 0.0f, 0.f));
	myEntityFloor->GetComponent<RenderableComponent>()->_canRenderShadows = true;*/

	playerEntity->GetTransform()->SetPosition(Vector3(2.0f, 0.0f, 15.0f));
	playerEntity->GetTransform()->SetScale(Vector3(0.01f, 0.01f, 0.01f));
	playerEntity->SetTag(Tags::PLAYER);

	enemyEntity1->GetTransform()->SetPosition(Vector3(10.0f, 0.0f, 26.0f));
	enemyEntity1->GetTransform()->SetScale(Vector3(0.009f, 0.009f, 0.009f));
	enemyEntity1->SetTag(Tags::ENEMY);

	enemyEntity2->GetTransform()->SetPosition(Vector3(-7.0f, 0.0f, 26.0f));
	enemyEntity2->GetTransform()->SetScale(Vector3(0.009f, 0.009f, 0.009f));
	enemyEntity2->SetTag(Tags::ENEMY);

	enemyEntity3->GetTransform()->SetPosition(Vector3(15.0f, 0.0f, 43.0f));
	enemyEntity3->GetTransform()->SetScale(Vector3(0.009f, 0.009f, 0.009f));
	enemyEntity3->SetTag(Tags::ENEMY);

	enemyEntity4->GetTransform()->SetPosition(Vector3(11.0f, 0.0f, 46.0f));
	enemyEntity4->GetTransform()->SetScale(Vector3(0.009f, 0.009f, 0.009f));
	enemyEntity4->SetTag(Tags::ENEMY);

	enemyEntity5->GetTransform()->SetPosition(Vector3(10.0f, 0.0f, 42.0f));
	enemyEntity5->GetTransform()->SetScale(Vector3(0.009f, 0.009f, 0.009f));
	enemyEntity5->SetTag(Tags::ENEMY);

	enemyEntity6->GetTransform()->SetPosition(Vector3(10.0f, 0.0f, 62.0f));
	enemyEntity6->GetTransform()->SetScale(Vector3(0.010f, 0.010f, 0.010f));
	enemyEntity6->SetTag(Tags::ENEMY);

	// Setting up parameters of audio -- REMOVE
	for (auto component : world->GetComponents<AudioComponent>())
	{
		if (strcmp(component->GetParent()->GetName().c_str(),
			"BackgroundAudio") == 0)
		{
			audioBackgroundSound = component;
			audioBackgroundSound->Loop = true;
			audioBackgroundSound->Volume = 0.1f;
			audioBackgroundSound->Mute = false;
			continue;
		}

		if (strcmp(component->GetParent()->GetName().c_str(),
			"DamageAudio") == 0)
		{		
			component->Volume = 0.2f;
			playerEntity->GetComponent<PlayerComponent>()->damageAudio = component;
			continue;
		}
		if (strcmp(component->GetParent()->GetName().c_str(),
			"PlayerFootstepAudio") == 0)
		{
			playerEntity->GetComponent<PlayerComponent>()->footstepAudio = component;
			continue;
		}
		if (strcmp(component->GetParent()->GetName().c_str(),
			"EnemyFootstepAudio") == 0)
		{
			component->Volume = 0.1f;
			enemyEntity1->GetComponent<EnemyComponent>()->footstepAudio = component;
			enemyEntity2->GetComponent<EnemyComponent>()->footstepAudio = component;
			enemyEntity3->GetComponent<EnemyComponent>()->footstepAudio = component;
			enemyEntity4->GetComponent<EnemyComponent>()->footstepAudio = component;
			enemyEntity5->GetComponent<EnemyComponent>()->footstepAudio = component;
			enemyEntity6->GetComponent<EnemyComponent>()->footstepAudio = component;
			continue;
		}
		if (strcmp(component->GetParent()->GetName().c_str(),
			"SwordSlashAudio") == 0)
		{
			component->Volume = 0.1f;
			playerEntity->GetComponent<PlayerComponent>()->swordAudio = component;
			continue;
		}
	}


	//// Setting up parameters of colliders ----------------------------------------------------------------

	//for (auto physicsComponent : world->GetComponents<PhysicsComponent>())
	//{
	//	if (strcmp(physicsComponent->GetParent()->GetName().c_str(), "Cup1") == 0)
	//	{
	//		colliderCup1 = physicsComponent;
	//		continue;
	//	}

	//	if (strcmp(physicsComponent->GetParent()->GetName().c_str(), "Cup2") == 0)
	//	{
	//		colliderCup2 = physicsComponent;
	//		continue;
	//	}
	//}

	//colliderBoundingCup1 = std::dynamic_pointer_cast<ColliderAABB>(colliderCup1->ColliderBounding);
	//colliderBoundingCup2 = std::dynamic_pointer_cast<ColliderSphere>(colliderCup2->ColliderBounding);

	// Setting up terrain tile map -------------------------------------------------------------------
	terrain->Initialize(context, device, playerEntity);


	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"roomtexture.dds",
			nullptr, m_roomTex.ReleaseAndGetAddressOf())); //REMOVE

	menuIsOn = false;


	// Initialization of systems ---------------------------------------------------------------------
	world->InitializeSystem<AudioSystem>();
	world->InitializeSystem<PhysicsSystem>();
	world->InitializeSystem<RenderableSystem>();
	//lightSystem->_fxFactory = renderableSystem->_fxFactory;
	world->InitializeSystem<LightSystem>();
	world->InitializeSystem<EnemySystem>();
	world->InitializeSystem<PlayerSystem>();


	// ----------------------   AFTER INITIALIZATION   -----------------------------------------------
	playerSystem->AdditionalInitialization(terrain, cooldown);
	enemySystem->AdditionalInitialization(playerEntity, terrain, playerSystem->playerHealth);

	//Setting up UI -----------------------------------------------------------------------------------
	Ui = make_shared<UI>(device, context, playerSystem->playerHealthOrigin, playerSystem->playerHealth);

	////Setting up skinned model -----------------------------------------------------------------------
	auto component = playerEntity->GetComponent<RenderableComponent>();
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Idle.fbx", "Idle");
	//component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Walk.fbx", "Walk");
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_HipHop.fbx", "HipHop");
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Dance.fbx", "Dance");
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Attack.fbx", "Attack");
	//component->_modelSkinned->AddAnimationClip("content\\Models\\Hero_Bite.fbx", "Bite");

	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Idle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Run_Slow.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_NormalAttack.fbx", "Attack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Bite.fbx", "Bite");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Rip.fbx", "Rip");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_PowerAttack_Normal.fbx", "PowerAttack");

	component = enemyEntity1->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");

	component = enemyEntity2->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");

	component = enemyEntity3->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");

	component = enemyEntity4->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");

	component = enemyEntity5->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	component->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");


	component = enemyEntity6->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardIdle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardWalk.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardAttack.fbx", "Attack");		// 1.8s;
	component->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardDying.fbx", "Dying");


	Ui->Initialize();


	// Normal map
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalText;
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"wall_normal.dds",
			nullptr,
			normalText.ReleaseAndGetAddressOf()));
	renderableSystem->_ShadowsfxFactory->SetNormalMap("initialShadingGroup", normalText.Get());


	playerEntity->GetComponent<RenderableComponent>()->_modelSkinned->GetAnimatorPlayer()->myModelName = "Player";
	playerEntity->GetComponent<RenderableComponent>()->_modelSkinned->GetAnimatorPlayer()->SetBlending(true);

	//world->RefreshWorld();
	renderableSystem->_terrain = terrain;
	renderableSystem->_camera = &camera;
}


void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
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

	Ui->Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}

void Game::ClearColorChanger()
{
	if (remPlotStage != plotStage)
	{
		remPlotStage = plotStage;
		ColorChanger = 0.0f;
	}
}

void Game::ShowPlot(int stage)
{
	ClearColorChanger();

	switch (stage)
	{
		case 1:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(blackBackTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot1Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 2:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(plot1Texture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot2Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 3:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(blackBackTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot3Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 4:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(blackBackTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot4Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 5:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(plot4Texture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot5Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 6:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(plot5Texture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot6Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 7:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(blackBackTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot7Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 8:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(plot7Texture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot8Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 9:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(blackBackTexture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot9Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}
		case 10:
		{
			m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

			m_spriteBatch->Draw(plot9Texture.Get(), m_screenPos, nullptr, Colors::White,
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->Draw(plot10Texture.Get(), m_screenPos, nullptr, XMVECTOR{ 1.0f, 1.0f, 1.0f, ColorChanger },
				0.f, Vector2(0, 0), 1.0f);

			m_spriteBatch->End();

			break;
		}

		default:
			break;
	}
}

void Game::SkipPlot()
{
	auto keyboard = Input::GetKeyboardState();
	keyboardTracker.Update(keyboard);

	if ((keyboardTracker.IsKeyPressed(Keyboard::Keys::Space)) || (keyboardTracker.IsKeyPressed(Keyboard::Keys::Escape)) || (keyboardTracker.IsKeyPressed(Keyboard::Keys::Enter)))
	{
		plotScreens = false;
	}
}

void Game::SkipStartScreen()
{
	auto keyboard = Input::GetKeyboardState();
	keyboardTracker.Update(keyboard);

	if ((keyboardTracker.IsKeyPressed(Keyboard::Keys::Space)) || (keyboardTracker.IsKeyPressed(Keyboard::Keys::Escape)) || (keyboardTracker.IsKeyPressed(Keyboard::Keys::Enter)))
	{
		gameStage = 3;
		startTimer += 30.0f;
	}
}

#pragma endregion