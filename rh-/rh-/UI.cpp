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
	vector<string> uiNames = {
		"healthBar", "healthAmount", "heroIconNormal", "vampireModeBorder",
		"normalAttack", "strongAttack", "spinAttack", "biteAttack",
		"teleport", "cleaveAttack", "swap","aoeAttack",
		"fpsBackground", "popUpMenu", "heroIconVampire"};

	map<string, string> uiNameTexMap = {
		{uiNames[0], "Resources\\UISprites\\hp_bar.dds"},
		{uiNames[1], "Resources\\UISprites\\health.dds"},
		{uiNames[2], "Resources\\UISprites\\Hero_Circle_Normal.dds"},
		{uiNames[3], "Resources\\UISprites\\red_border.dds"},
		{uiNames[4], "Resources\\UISprites\\Normal_Attack.dds"},
		{uiNames[5], "Resources\\UISprites\\Strong_Attack.dds"},
		{uiNames[6], "Resources\\UISprites\\Spin_Attack.dds"},
		{uiNames[7], "Resources\\UISprites\\Bite_Attack.dds"},
		{uiNames[8], "Resources\\UISprites\\Teleport.dds"},
		{uiNames[9], "Resources\\UISprites\\Cleave_Attack.dds"},
		{uiNames[10], "Resources\\UISprites\\Swap.dds"},
		{uiNames[11], "Resources\\UISprites\\Aoe_Attack.dds"},
		{uiNames[12], "Resources\\UISprites\\fpsbar.dds"},
		{uiNames[13], "Resources\\UISprites\\Menu.dds"},
		{uiNames[14], "Resources\\UISprites\\Hero_Circle_Vampire.dds"}
	};

	skillSetPosition = Vector2(650.0f, 800.0f);

	map<string, Vector2> uiNamePositionMap{
		{uiNames[0], Vector2(0.0f, 0.0f)},
		{uiNames[1], Vector2(135.f, 30.0f)},
		{uiNames[2], Vector2(0.0f, 0.0f)},
		{uiNames[3], Vector2(0.0f, 0.0f)},
		{uiNames[4], skillSetPosition},
		{uiNames[5], Vector2(skillSetPosition.x + 150.0f, skillSetPosition.y)},
		{uiNames[6], Vector2(skillSetPosition.x + 300.0f, skillSetPosition.y)},
		{uiNames[7], Vector2(skillSetPosition.x + 450.0f, skillSetPosition.y)},
		{uiNames[8], skillSetPosition},
		{uiNames[9], Vector2(skillSetPosition.x + 150.0f, skillSetPosition.y)},
		{uiNames[10], Vector2(skillSetPosition.x + 300.0f, skillSetPosition.y)},
		{uiNames[11], Vector2(skillSetPosition.x + 450.0f, skillSetPosition.y)},
		{uiNames[12], Vector2(710.0f, -5.0f)},
		{uiNames[13], Vector2(250.0f, 100.0f)},
		{uiNames[14], Vector2(0.0f, 0.0f)},
	};

	map<string, Vector2> uiNameScaleMap{
		{uiNames[0], Vector2(0.25f, 0.25f)},
		{uiNames[1], Vector2(0.3f, 0.3f)},
		{uiNames[2], Vector2(0.35f, 0.35f)},
		{uiNames[3], Vector2(1.0f, 1.0f)},
		{uiNames[4], Vector2(0.3f, 0.3f)},
		{uiNames[5], Vector2(0.3f, 0.3f)},
		{uiNames[6], Vector2(0.3f, 0.3f)},
		{uiNames[7], Vector2(0.3f, 0.3f)},
		{uiNames[8], Vector2(0.3f, 0.3f)},
		{uiNames[9], Vector2(0.3f, 0.3f)},
		{uiNames[10], Vector2(0.3f, 0.3f)},
		{uiNames[11], Vector2(0.3f, 0.3f)},
		{uiNames[12], Vector2(0.15f, 0.15f)},
		{uiNames[13], Vector2(0.6f, 0.6f)},
		{uiNames[14], Vector2(0.35f, 0.35f)},

	};


	for each(string uiElement in uiNames)
	{
		_elements[uiElement] = UiElement();

		_elements[uiElement].name = uiElement;

		wstring wide_string = wstring(uiNameTexMap[uiElement].begin(), uiNameTexMap[uiElement].end());
		const wchar_t* spritePath = wide_string.c_str();
		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(_device, spritePath,
				nullptr,
				_elements[uiElement].texture.ReleaseAndGetAddressOf()));

		_elements[uiElement].position = uiNamePositionMap[uiElement];
		_elements[uiElement].scale = uiNameScaleMap[uiElement];
	}

	uiSpriteBatch = std::make_shared<SpriteBatch>(_context); // UI Component, UISystem->Initialize()
	uiSpriteBatchBorder = std::make_shared<SpriteBatch>(_context);

	fpsFont = std::make_unique<SpriteFont>(_device, L"Resources\\Fonts\\fpsFont.spritefont");
	fpsFontPos = Vector2(710.0f, 10.0f);
	fpsFontText = std::wstring(L"60");

}

void UI::DrawRedBorder()
{
	uiSpriteBatchBorder->Begin();

	uiSpriteBatchBorder->Draw(_elements["vampireModeBorder"].texture.Get(), _elements["vampireModeBorder"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _elements["vampireModeBorder"].scale);

	uiSpriteBatchBorder->End();
}

void UI::Draw(bool vampireMode, bool menuIsOn)
{
	_elements["healthAmount"].scale.x = ((_elements["healthAmount"].scale.y * (*_playerHealth)) / _playerHealthOrigin);
	if (_elements["healthAmount"].scale.x < 0)
		_elements["healthAmount"].scale.x = 0;
	else if (_elements["healthAmount"].scale.x > _playerHealthOrigin)
		_elements["healthAmount"].scale.x = _elements["healthAmount"].scale.y;

	uiSpriteBatch->Begin();
	uiSpriteBatch->Draw(_elements["healthBar"].texture.Get(), _elements["healthBar"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _elements["healthBar"].scale);

	uiSpriteBatch->Draw(_elements["healthAmount"].texture.Get(), _elements["healthAmount"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _elements["healthAmount"].scale);


	if (!vampireMode)
	{
		uiSpriteBatch->Draw(_elements["heroIconNormal"].texture.Get(), _elements["heroIconNormal"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["heroIconNormal"].scale);

		uiSpriteBatch->Draw(_elements["normalAttack"].texture.Get(), _elements["normalAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["normalAttack"].scale);

		uiSpriteBatch->Draw(_elements["strongAttack"].texture.Get(), _elements["strongAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["strongAttack"].scale);

		uiSpriteBatch->Draw(_elements["spinAttack"].texture.Get(), _elements["spinAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["spinAttack"].scale);

		uiSpriteBatch->Draw(_elements["biteAttack"].texture.Get(), _elements["biteAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["biteAttack"].scale);
	}
	else
	{
		DrawRedBorder();

		uiSpriteBatch->Draw(_elements["heroIconVampire"].texture.Get(), _elements["heroIconVampire"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["heroIconVampire"].scale);

		uiSpriteBatch->Draw(_elements["teleport"].texture.Get(), _elements["teleport"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["teleport"].scale);

		uiSpriteBatch->Draw(_elements["cleaveAttack"].texture.Get(), _elements["cleaveAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["cleaveAttack"].scale);

		uiSpriteBatch->Draw(_elements["swap"].texture.Get(), _elements["swap"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["swap"].scale);

		uiSpriteBatch->Draw(_elements["aoeAttack"].texture.Get(), _elements["aoeAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["aoeAttack"].scale);
	}

	uiSpriteBatch->Draw(_elements["fpsBackground"].texture.Get(), _elements["fpsBackground"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _elements["fpsBackground"].scale);

	fpsFont->DrawString(uiSpriteBatch.get(), fpsFontText.c_str(),
		fpsFontPos, Colors::Black, 0.f, Vector2(0, 0));

	if (menuIsOn)
	{
		uiSpriteBatch->Draw(_elements["popUpMenu"].texture.Get(), _elements["popUpMenu"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["popUpMenu"].scale);
	}

	uiSpriteBatch->End();
}

void UI::Reset()
{
	uiSpriteBatch.reset();
	uiSpriteBatchBorder.reset();
	fpsFont.reset();

	for (std::map<string, UiElement>::iterator it = _elements.begin(); it != _elements.end(); ++it)
	{
		it->second.texture.Reset();
	}
}

