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
		"swapCost", "skillKeyLPM", "skillKeyPPM", "skillKeyCPM", "skillKeyE",
		"skillKey1", "skillKey2", "skillKey3", "skillKey4"
	};

	map<string, string> uiNameTexMap = {
		{"healthBar", "Resources\\UISprites\\hp_bar.dds"},
		{"healthAmount", "Resources\\UISprites\\Blood_Drop.dds"},
		{"heroIconNormal", "Resources\\UISprites\\Hero_Circle_Normal.dds"},
		{"vampireModeBorder", "Resources\\UISprites\\red_border.dds"},
		{"normalAttack", "Resources\\UISprites\\Normal_Attack.dds"},
		{"strongAttack", "Resources\\UISprites\\Strong_Attack.dds"},
		{"spinAttack", "Resources\\UISprites\\Spin_Attack.dds"},
		{"biteAttack", "Resources\\UISprites\\Bite_Attack.dds"},
		{"teleport", "Resources\\UISprites\\Teleport.dds"},
		{"cleaveAttack", "Resources\\UISprites\\Cleave_Attack.dds"},
		{"swap", "Resources\\UISprites\\Swap.dds"},
		{"aoeAttack", "Resources\\UISprites\\Aoe_Attack.dds"},
		{"fpsBackground", "Resources\\UISprites\\fpsbar.dds"},
		{"popUpMenu", "Resources\\UISprites\\Menu.dds"},
		{"heroIconVampire", "Resources\\UISprites\\Hero_Circle_Vampire.dds"},
		{"heroIconVampireRing", "Resources\\UISprites\\Hero_Circle_Vampire_Ring.dds"},
		{"heroIconTransitionRing", "Resources\\UISprites\\Hero_Circle_Transition.dds"},
		{"vamprireRedCircle", "Resources\\UISprites\\Vampire_Red_Circle.dds"},
		{"teleportCost", "Resources\\UISprites\\Blood_Drop_One.dds"},
		{"cleaveAttackCost", "Resources\\UISprites\\Blood_Drop_Two.dds"},
		{"swapCost", "Resources\\UISprites\\Blood_Drop_One.dds"},
		{"skillKeyLPM", "Resources\\UISprites\\Human_Skill_Key_LPM.dds"},
		{"skillKeyPPM", "Resources\\UISprites\\Human_Skill_Key_PPM.dds"},
		{"skillKeyCPM", "Resources\\UISprites\\Human_Skill_Key_CPM.dds"},
		{"skillKeyE", "Resources\\UISprites\\Human_Skill_Key_E.dds"},
		{"skillKey1", "Resources\\UISprites\\Vampire_Skill_Key_1.dds"},
		{"skillKey2", "Resources\\UISprites\\Vampire_Skill_Key_2.dds"},
		{"skillKey3", "Resources\\UISprites\\Vampire_Skill_Key_3.dds"},
		{"skillKey4", "Resources\\UISprites\\Vampire_Skill_Key_4.dds"}
	};

	skillSetPosition = Vector2(690.0f, 930.0f);

	map<string, Vector2> uiNamePositionMap{
		{"healthBar", Vector2(0.0f, 0.0f)},
		{"healthAmount", Vector2(140.f, 28.0f)},
		{"heroIconNormal", Vector2(0.0f, 0.0f)},
		{"vampireModeBorder", Vector2(0.0f, 0.0f)},
		{"normalAttack", skillSetPosition + Vector2(5.0f, 0.0f)},
		{"strongAttack", Vector2(skillSetPosition.x + 155.0f, skillSetPosition.y)},
		{"spinAttack", Vector2(skillSetPosition.x + 305.0f, skillSetPosition.y)},
		{"biteAttack", Vector2(skillSetPosition.x + 455.0f, skillSetPosition.y)},
		{"teleport", skillSetPosition + Vector2(-5.0f, -10.0f)},
		{"cleaveAttack", Vector2(skillSetPosition.x + 145.0f, skillSetPosition.y - 10.0f)},
		{"swap", Vector2(skillSetPosition.x + 295.0f, skillSetPosition.y - 10.0f)},
		{"aoeAttack", Vector2(skillSetPosition.x + 445.0f, skillSetPosition.y - 10.0f)},
		{"fpsBackground", Vector2(710.0f, -5.0f)},
		{"popUpMenu", Vector2(250.0f, 100.0f)},
		{"heroIconVampire", Vector2(0.0f, 0.0f)},
		{"heroIconVampireRing", Vector2(0.0f, 0.0f)},
		{"heroIconTransitionRing", Vector2(0.0f, 0.0f)},
		{"vamprireRedCircle", skillSetPosition + Vector2(-6.0f, -12.0f)},
		{"teleportCost", skillSetPosition + Vector2(25.0f, -60.0f)},
		{"cleaveAttackCost", Vector2(skillSetPosition.x + 145.0f + 15.0f, skillSetPosition.y - 60.0f)},
		{"swapCost", Vector2(skillSetPosition.x + 290.0f + 35.0f, skillSetPosition.y - 60.0f)},
		{"skillKeyLPM", skillSetPosition + Vector2(15.0f, 65.0f)},
		{"skillKeyPPM", skillSetPosition + Vector2(165.0f, 65.0f)},
		{"skillKeyCPM", skillSetPosition + Vector2(315.0f, 65.0f)},
		{"skillKeyE", skillSetPosition + Vector2(465.0f, 65.0f)},
		{"skillKey1", skillSetPosition + Vector2(15.0f, 65.0f)},
		{"skillKey2", skillSetPosition + Vector2(165.0f, 65.0f)},
		{"skillKey3", skillSetPosition + Vector2(315.0f, 65.0f)},
		{"skillKey4", skillSetPosition + Vector2(465.0f, 65.0f)}
	};

	map<string, Vector2> uiNameScaleMap{
		{"healthBar", Vector2(0.25f, 0.25f)},
		{"healthAmount", Vector2(0.10f, 0.10f)},
		{"heroIconNormal", Vector2(0.35f, 0.35f)},
		{"vampireModeBorder", Vector2(1.0f, 1.0f)},
		{"normalAttack", Vector2(0.20f, 0.20f)},
		{"strongAttack", Vector2(0.20f, 0.20f)},
		{"spinAttack", Vector2(0.20f, 0.20f)},
		{"biteAttack", Vector2(0.20f, 0.20f)},
		{"teleport", Vector2(0.24f, 0.24f)},
		{"cleaveAttack", Vector2(0.24f, 0.24f)},
		{"swap", Vector2(0.24f, 0.24f)},
		{"aoeAttack", Vector2(0.24f, 0.24f)},
		{"fpsBackground", Vector2(0.15f, 0.15f)},
		{"popUpMenu", Vector2(0.6f, 0.6f)},
		{"heroIconVampire", Vector2(0.35f, 0.35f)},
		{"heroIconVampireRing", Vector2(0.35f, 0.35f)},
		{"heroIconTransitionRing", Vector2(0.35f, 0.35f)},
		{"vamprireRedCircle", Vector2(0.25f, 0.25f)},
		{"teleportCost", Vector2(0.25f, 0.25f)},
		{"cleaveAttackCost", Vector2(0.25f, 0.25f)},
		{"swapCost", Vector2(0.25f, 0.25f)},
		{"skillKeyLPM", Vector2(0.30f, 0.30f)},
		{"skillKeyPPM", Vector2(0.30f, 0.30f)},
		{"skillKeyCPM", Vector2(0.30f, 0.30f)},
		{"skillKeyE", Vector2(0.30f, 0.30f)},
		{"skillKey1", Vector2(0.30f, 0.30f)},
		{"skillKey2", Vector2(0.30f, 0.30f)},
		{"skillKey3", Vector2(0.30f, 0.30f)},
		{"skillKey4", Vector2(0.30f, 0.30f)}
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

		uiSpriteBatch->Draw(_elements["skillKeyLPM"].texture.Get(), _elements["skillKeyLPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKeyLPM"].scale);

		uiSpriteBatch->Draw(_elements["skillKeyPPM"].texture.Get(), _elements["skillKeyPPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKeyPPM"].scale);

		uiSpriteBatch->Draw(_elements["skillKeyCPM"].texture.Get(), _elements["skillKeyCPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKeyCPM"].scale);

		uiSpriteBatch->Draw(_elements["skillKeyE"].texture.Get(), _elements["skillKeyE"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKeyE"].scale);

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

		uiSpriteBatch->Draw(_elements["skillKey1"].texture.Get(), _elements["skillKey1"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKey1"].scale);

		uiSpriteBatch->Draw(_elements["skillKey2"].texture.Get(), _elements["skillKey2"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKey2"].scale);

		uiSpriteBatch->Draw(_elements["skillKey3"].texture.Get(), _elements["skillKey3"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKey3"].scale);

		uiSpriteBatch->Draw(_elements["skillKey4"].texture.Get(), _elements["skillKey4"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _elements["skillKey4"].scale);

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

