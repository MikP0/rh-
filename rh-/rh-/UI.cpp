#include "pch.h"
#include "UI.h"

UI::UI(ID3D11Device1 * device, ID3D11DeviceContext1 * context, float playerHealthOrigin, shared_ptr<float> playerHealth)
{
	_device = device;
	_context = context;
	_playerHealthOrigin = playerHealthOrigin;
	_playerHealth = playerHealth;
	transitionMode = false;
	transitionElapsedTime = 0.0f;
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
		"fpsBackground", "popUpMenu", "heroIconVampire", "heroIconVampireRing",
		"heroIconTransitionRing", "vamprireRedCircle", "teleportCost", "cleaveAttackCost",
		"swapCost"};

	map<string, string> uiNameTexMap = {
		{uiNames[0], "Resources\\UISprites\\hp_bar.dds"},
		{uiNames[1], "Resources\\UISprites\\Blood_Drop.dds"},
		{uiNames[2], "Resources\\UISprites\\Hero_Circle_Normal.dds"},
		{uiNames[3], "Resources\\UISprites\\red_border.dds"},
		{uiNames[4], "Resources\\UISprites\\Normal_Attack.dds"},
		{uiNames[5], "Resources\\UISprites\\Strong_Attack.dds"},
		{uiNames[6], "Resources\\UISprites\\Human_Blocked_Skill.dds"},
		{uiNames[7], "Resources\\UISprites\\Bite_Attack.dds"},
		{uiNames[8], "Resources\\UISprites\\Teleport.dds"},
		{uiNames[9], "Resources\\UISprites\\Cleave_Attack.dds"},
		{uiNames[10], "Resources\\UISprites\\Swap.dds"},
		{uiNames[11], "Resources\\UISprites\\Vampire_Blocked_Skill.dds"},
		{uiNames[12], "Resources\\UISprites\\fpsbar.dds"},
		{uiNames[13], "Resources\\UISprites\\Menu.dds"},
		{uiNames[14], "Resources\\UISprites\\Hero_Circle_Vampire.dds"},
		{uiNames[15], "Resources\\UISprites\\Hero_Circle_Vampire_Ring.dds"},
		{uiNames[16], "Resources\\UISprites\\Hero_Circle_Transition.dds"},
		{uiNames[17], "Resources\\UISprites\\Vampire_Red_Circle.dds"},
		{uiNames[18], "Resources\\UISprites\\Blood_Drop_One.dds"},
		{uiNames[19], "Resources\\UISprites\\Blood_Drop_Two.dds"},
		{uiNames[20], "Resources\\UISprites\\Blood_Drop_One.dds"}
	};

	skillSetPosition = Vector2(650.0f, 850.0f);

	map<string, Vector2> uiNamePositionMap{
		{uiNames[0], Vector2(0.0f, 0.0f)},
		{uiNames[1], Vector2(140.f, 28.0f)},
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
		{uiNames[15], Vector2(0.0f, 0.0f)},
		{uiNames[16], Vector2(0.0f, 0.0f)},
		{uiNames[17], skillSetPosition},
		{uiNames[18], skillSetPosition + Vector2(35.0f, -60.0f)},
		{uiNames[19], Vector2(skillSetPosition.x + 150.0f + 15.0f, skillSetPosition.y - 60.0f)},
		{uiNames[20], Vector2(skillSetPosition.x + 300.0f + 35.0f, skillSetPosition.y - 60.0f)}
	};

	map<string, Vector2> uiNameScaleMap{
		{uiNames[0], Vector2(0.25f, 0.25f)},
		{uiNames[1], Vector2(0.10f, 0.10f)},
		{uiNames[2], Vector2(0.35f, 0.35f)},
		{uiNames[3], Vector2(1.0f, 1.0f)},
		{uiNames[4], Vector2(0.25f, 0.25f)},
		{uiNames[5], Vector2(0.25f, 0.25f)},
		{uiNames[6], Vector2(0.25f, 0.25f)},
		{uiNames[7], Vector2(0.25f, 0.25f)},
		{uiNames[8], Vector2(0.27f, 0.27f)},
		{uiNames[9], Vector2(0.27f, 0.27f)},
		{uiNames[10], Vector2(0.27f, 0.27f)},
		{uiNames[11], Vector2(0.27f, 0.27f)},
		{uiNames[12], Vector2(0.15f, 0.15f)},
		{uiNames[13], Vector2(0.6f, 0.6f)},
		{uiNames[14], Vector2(0.35f, 0.35f)},
		{uiNames[15], Vector2(0.35f, 0.35f)},
		{uiNames[16], Vector2(0.35f, 0.35f)},
		{uiNames[17], Vector2(0.25f, 0.25f)},
		{uiNames[18], Vector2(0.25f, 0.25f)},
		{uiNames[19], Vector2(0.25f, 0.25f)},
		{uiNames[20], Vector2(0.25f, 0.25f)}
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

void UI::Draw(bool vampireMode, int selectedVampireAbility, bool menuIsOn, float totalTime, float elapsedTime)
{
	uiSpriteBatch->Begin();

	/*uiSpriteBatch->Draw(_elements["healthBar"].texture.Get(), _elements["healthBar"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _elements["healthBar"].scale);*/

	for (int i = 0; i < (int)*_playerHealth; i++)
	{
		uiSpriteBatch->Draw(_elements["healthAmount"].texture.Get(), _elements["healthAmount"].position + i * Vector2(40.0f, 0.0f), nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["healthAmount"].scale);
	}

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

		if (transitionMode)
		{
			uiSpriteBatch->Draw(_elements["heroIconTransitionRing"].texture.Get(), _elements["heroIconTransitionRing"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _elements["heroIconTransitionRing"].scale);

			transitionElapsedTime += elapsedTime;

			if (transitionElapsedTime >= 0.5f)
			{
				transitionMode = false;
				transitionElapsedTime = 0.0f;
			}
		} 
		else
		{
			uiSpriteBatch->Draw(_elements["heroIconVampire"].texture.Get(), _elements["heroIconVampire"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _elements["heroIconVampire"].scale);

			uiSpriteBatch->Draw(_elements["heroIconVampireRing"].texture.Get(), _elements["heroIconVampireRing"].position + Vector2(70.0f, 70.0f), nullptr, Colors::White,
				sinf(totalTime) * 6.0f, Vector2(206, 206), _elements["heroIconVampireRing"].scale);
		}

		uiSpriteBatch->Draw(_elements["teleport"].texture.Get(), _elements["teleport"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["teleport"].scale);

		uiSpriteBatch->Draw(_elements["cleaveAttack"].texture.Get(), _elements["cleaveAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["cleaveAttack"].scale);

		uiSpriteBatch->Draw(_elements["swap"].texture.Get(), _elements["swap"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["swap"].scale);

		uiSpriteBatch->Draw(_elements["aoeAttack"].texture.Get(), _elements["aoeAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["aoeAttack"].scale);

		if (selectedVampireAbility != 0)
		{
			Vector2 vampireRedCirclePos = _elements["vamprireRedCircle"].position + Vector2(150.0f * (selectedVampireAbility - 1), 0.0f);

			uiSpriteBatch->Draw(_elements["vamprireRedCircle"].texture.Get(), vampireRedCirclePos, nullptr, Colors::White,
				0.f, Vector2(0, 0), _elements["vamprireRedCircle"].scale);
		}

		uiSpriteBatch->Draw(_elements["teleportCost"].texture.Get(), _elements["teleportCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["teleportCost"].scale);

		uiSpriteBatch->Draw(_elements["cleaveAttackCost"].texture.Get(), _elements["cleaveAttackCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["cleaveAttackCost"].scale);

		uiSpriteBatch->Draw(_elements["swapCost"].texture.Get(), _elements["swapCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["swapCost"].scale);

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

