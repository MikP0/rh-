#include "pch.h"
#include "UI.h"

UI::UI(ID3D11Device1 * device, ID3D11DeviceContext1 * context, float playerHealthOrigin, shared_ptr<float> playerHealth)
{
	_device = device;
	_context = context;
	_playerHealthOrigin = playerHealthOrigin;
	_playerHealth = playerHealth;
}

UI::~UI()
{
}

void UI::Initialize()
{
	uiSpriteBatch = std::make_shared<SpriteBatch>(_context); // UI Component, UISystem->Initialize()
	uiSpriteBatchBorder = std::make_shared<SpriteBatch>(_context);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\hp_bar.dds",
			nullptr,
			healthBarTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Hero_Circle.dds",
			nullptr,
			healthBarHeroTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\health.dds",
			nullptr,
			healthBarHealthTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\red_border.dds",
			nullptr,
			redBorderTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Normal_Attack.dds",
			nullptr,
			normalAttackTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Power_Attack.dds",
			nullptr,
			powerAttackTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Spin.dds",
			nullptr,
			spinAttackTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Bite.dds",
			nullptr,
			biteAttackTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Dash.dds",
			nullptr,
			dashTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Rip.dds",
			nullptr,
			ripTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Swap.dds",
			nullptr,
			swapAttackTex.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Aoe.dds",
			nullptr,
			aoeAttackTex.ReleaseAndGetAddressOf()));


	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Hero_Circle.dds",
			nullptr,
			heroCircleTex.ReleaseAndGetAddressOf()));


	healthBarPos.x = 0.f;
	healthBarPos.y = 0.f;

	healthBarHeroPos.x = 0.f;
	healthBarHeroPos.y = 0.f;

	healthBarHealthPos.x = 135.f;
	healthBarHealthPos.y = 30.f;
	healthBarHealthScale.x = 0.25f;
	healthBarHealthScale.y = 0.25f;

	redBorderPos.x = 0.0f;
	redBorderPos.y = 0.0f;
	redBorderScale.x = 1.91f;
	redBorderScale.y = 1.36f;

	skillSetPosition.x = 650.0f;
	skillSetPosition.y = 800.0f;

	normalAttackPos.x = skillSetPosition.x + 0.0f;
	normalAttackPos.y = skillSetPosition.y + 0.0f;
	normalAttackScale.x = 0.3f;
	normalAttackScale.y = 0.3f;

	powerAttackPos.x = skillSetPosition.x + 150.0f;
	powerAttackPos.y = skillSetPosition.y + 0.0f;
	powerAttackScale.x = 0.3f;
	powerAttackScale.y = 0.3f;

	spinAttackPos.x = skillSetPosition.x + 300.0f;
	spinAttackPos.y = skillSetPosition.y + 0.0f;
	spinAttackScale.x = 0.3f;
	spinAttackScale.y = 0.3f;

	biteAttackPos.x = skillSetPosition.x + 450.0f;
	biteAttackPos.y = skillSetPosition.y + 0.0f;
	biteAttackScale.x = 0.3f;
	biteAttackScale.y = 0.3f;

	dashPos.x = skillSetPosition.x + 0.0f;
	dashPos.y = skillSetPosition.y + 0.0f;
	dashScale.x = 0.3f;
	dashScale.y = 0.3f;

	ripAttackPos.x = skillSetPosition.x + 150.0f;
	ripAttackPos.y = skillSetPosition.y + 0.0f;
	ripAttackScale.x = 0.3f;
	ripAttackScale.y = 0.3f;

	swapAttackPos.x = skillSetPosition.x + 300.0f;
	swapAttackPos.y = skillSetPosition.y + 0.0f;
	swapAttackScale.x = 0.3f;
	swapAttackScale.y = 0.3f;

	aoeAttackPos.x = skillSetPosition.x + 450.0f;
	aoeAttackPos.y = skillSetPosition.y + 0.0f;
	aoeAttackScale.x = 0.3f;
	aoeAttackScale.y = 0.3f;


	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\fpsbar.dds",
			nullptr,
			fpsBarTex.ReleaseAndGetAddressOf()));

	fpsBarPos.x = 710.0f;
	fpsBarPos.y = -5.0f;

	fpsFont = std::make_unique<SpriteFont>(_device, L"Resources\\Fonts\\fpsFont.spritefont");

	fpsFontPos.x = 710.0f;
	fpsFontPos.y = 10.0f;
	fpsFontText = std::wstring(L"60");

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(_device, L"Resources\\UISprites\\Menu.dds",
			nullptr,
			menuTex.ReleaseAndGetAddressOf()));

	menuPos.x = 250.0f;
	menuPos.y = 100.0f;

}

void UI::DrawRedBorder()
{
	uiSpriteBatchBorder->Begin();

	uiSpriteBatchBorder->Draw(redBorderTex.Get(), redBorderPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), redBorderScale);

	uiSpriteBatchBorder->End();
}

void UI::Draw(bool vampireMode, bool menuIsOn)
{
	uiSpriteBatch->Begin();

	// show depth map
	//uiSpriteBatch->Draw(renderableSystem->_shadowMap->GetDepthMapSRV(), Vector2(850, 650), nullptr, Colors::White, 0.f, Vector2(0, 0), 0.3f);

	uiSpriteBatch->Draw(healthBarTex.Get(), healthBarPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.25f);


	healthBarHealthScale.x = ((healthBarHealthScale.y * (*_playerHealth)) / _playerHealthOrigin);
	if (healthBarHealthScale.x < 0)
		healthBarHealthScale.x = 0;
	else if (healthBarHealthScale.x > _playerHealthOrigin)
		healthBarHealthScale.x = healthBarHealthScale.y;


	uiSpriteBatch->Draw(healthBarHealthTex.Get(), healthBarHealthPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), healthBarHealthScale);

	uiSpriteBatch->Draw(healthBarHeroTex.Get(), healthBarHeroPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.35f);

	if (!vampireMode)
	{
		uiSpriteBatch->Draw(normalAttackTex.Get(), normalAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), normalAttackScale);

		uiSpriteBatch->Draw(powerAttackTex.Get(), powerAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), powerAttackScale);

		uiSpriteBatch->Draw(spinAttackTex.Get(), spinAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), spinAttackScale);

		uiSpriteBatch->Draw(biteAttackTex.Get(), biteAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), biteAttackScale);
	}
	else
	{
		DrawRedBorder();

		uiSpriteBatch->Draw(dashTex.Get(), dashPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), dashScale);

		uiSpriteBatch->Draw(ripTex.Get(), ripAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), ripAttackScale);

		uiSpriteBatch->Draw(swapAttackTex.Get(), swapAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), swapAttackScale);

		uiSpriteBatch->Draw(aoeAttackTex.Get(), aoeAttackPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), aoeAttackScale);
	}

	/*uiSpriteBatch->Draw(fpsBarTex.Get(), fpsBarPos, nullptr, Colors::White,
		0.f, Vector2(0, 0), 0.15f);

	fpsFont->DrawString(uiSpriteBatch.get(), fpsFontText.c_str(),
		fpsFontPos, Colors::Black, 0.f, Vector2(0, 0));*/

	if (menuIsOn)
	{
		uiSpriteBatch->Draw(menuTex.Get(), menuPos, nullptr, Colors::White,
			0.f, Vector2(0, 0), 0.6f);
	}

	uiSpriteBatch->End();
}

void UI::Reset()
{
	uiSpriteBatch.reset();
	healthBarTex.Reset();
	healthBarHeroTex.Reset();
	healthBarHealthTex.Reset();
	redBorderTex.Reset();
	normalAttackTex.Reset();
	powerAttackTex.Reset();
	spinAttackTex.Reset();
	biteAttackTex.Reset();
	swapAttackTex.Reset();
	aoeAttackTex.Reset();
	dashTex.Reset();
	heroCircleTex.Reset();
	ripTex.Reset();
}

