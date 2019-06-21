#include "pch.h"
#include "UI.h"

UI::UI(ID3D11Device1 * device, ID3D11DeviceContext1 * context, shared_ptr<PlayerSystem> playerSystem)
{
	_device = device;
	_context = context;
	transitionMode = false;
	transitionElapsedTime = 0.0f;
	_playerSystem = playerSystem;
}

UI::~UI()
{
}

void UI::Initialize()
{
	vector<string> uiImageNames = {
		"healthBar", "healthAmount", "healthAmountSkillCost", "heroIconNormal", "vampireModeBorder",
		"normalAttack", "strongAttack", "spinAttack", "biteAttack",
		"teleport", "cleaveAttack", "swap","aoeAttack",
		"fpsBackground", "popUpMenu", "heroIconVampire", "heroIconVampireRing",
		"heroIconTransitionRing", "vamprireRedCircle", "teleportCost", "cleaveAttackCost",
		"swapCost", "skillKeyLPM", "skillKeyPPM", "skillKeyCPM", "skillKeyE",
		"skillKey1", "skillKey2", "skillKey3", "skillKey4",
		"humanCoolDownFrame", "vampireCoolDownFrame"
	};

	vector<string> uiTextNames = {
		"normalAttackCooldown", "strongAttackCooldown",
		"spinAttackCooldown", "biteAttackCooldown"
	};

	map<string, string> uiImageNameTexMap = {
		{"healthBar", "Resources\\UISprites\\Bar.dds"},
		{"healthAmount", "Resources\\UISprites\\Blood_Drop.dds"},
		{"healthAmountSkillCost", "Resources\\UISprites\\Blood_Drop_Skill_Cost.dds"},
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
		{"skillKey4", "Resources\\UISprites\\Vampire_Skill_Key_4.dds"},
		{"humanCoolDownFrame", "Resources\\UISprites\\Human_Skill_Cooldown_Frame.dds"},
		{"vampireCoolDownFrame", "Resources\\UISprites\\Vampire_Skill_Cooldown_Frame.dds"}
	};

	skillSetPosition = Vector2(690.0f, 930.0f);

	map<string, Vector2> uiNamePositionMap = {
		{"healthBar", Vector2(95.f, 10.0f)},
		{"healthAmount", Vector2(140.f, 28.0f)},
		{"healthAmountSkillCost", Vector2(140.f, 28.0f)},
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
		{"skillKey4", skillSetPosition + Vector2(465.0f, 65.0f)},
		{"humanCoolDownFrame", skillSetPosition + Vector2(5.0f, 0.0f)},
		{"vampireCoolDownFrame", skillSetPosition + Vector2(-5.0f, -10.0f)},
		{"normalAttackCooldown", skillSetPosition + Vector2(14.0f, 5.0f)},
		{"strongAttackCooldown", skillSetPosition + Vector2(166.0f, 5.0f)},
		{"spinAttackCooldown", skillSetPosition + Vector2(316.0f, 5.0f)},
		{"biteAttackCooldown", skillSetPosition + Vector2(466.0f, 5.0f)}
	};

	map<string, Vector2> uiNameScaleMap = {
		{"healthBar", Vector2(0.5f, 0.18f)},
		{"healthAmount", Vector2(0.10f, 0.10f)},
		{"healthAmountSkillCost", Vector2(0.10f, 0.10f)},
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
		{"skillKey4", Vector2(0.30f, 0.30f)},
		{"humanCoolDownFrame", Vector2(0.20f, 0.20f)},
		{"vampireCoolDownFrame", Vector2(0.24f, 0.24f)},
		{"normalAttackCooldown", Vector2(1.6f, 1.6f)},
		{"strongAttackCooldown", Vector2(1.6f, 1.6f)},
		{"spinAttackCooldown", Vector2(1.6f, 1.6f)},
		{"biteAttackCooldown", Vector2(1.6f, 1.6f)}
	};


	for each(string uiElement in uiImageNames)
	{
		_imageElements[uiElement] = ImageUiElement();
		_imageElements[uiElement].name = uiElement;

		wstring wide_string = wstring(uiImageNameTexMap[uiElement].begin(), uiImageNameTexMap[uiElement].end());
		const wchar_t* spritePath = wide_string.c_str();
		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(_device, spritePath,
				nullptr,
				_imageElements[uiElement].texture.ReleaseAndGetAddressOf()));

		_imageElements[uiElement].position = uiNamePositionMap[uiElement];
		_imageElements[uiElement].scale = uiNameScaleMap[uiElement];
	}

	for each(string uiElement in uiTextNames)
	{
		_textElements[uiElement] = TextUiElement();
		_textElements[uiElement].name = uiElement;
		_textElements[uiElement].position = uiNamePositionMap[uiElement];
		_textElements[uiElement].scale = uiNameScaleMap[uiElement];
		_textElements[uiElement].font = make_unique<SpriteFont>(_device, L"Resources\\Fonts\\fpsFont.spritefont");
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

	uiSpriteBatchBorder->Draw(_imageElements["vampireModeBorder"].texture.Get(), _imageElements["vampireModeBorder"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _imageElements["vampireModeBorder"].scale);

	uiSpriteBatchBorder->End();
}

void UI::Draw(bool menuIsOn, float totalTime, float elapsedTime)
{
	bool vampireMode = _playerSystem->vampireMode;
	int vampireAbility = vampireMode != 0 ? _playerSystem->player->vampireAbility : 0;
	int bloodDropsToColor = 0;
	int bloodDropNumber;
	int playerCurrentHealth = (int)*_playerSystem->playerHealth;

	uiSpriteBatch->Begin();

	uiSpriteBatch->Draw(_imageElements["healthBar"].texture.Get(), _imageElements["healthBar"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _imageElements["healthBar"].scale);

	if (vampireMode && vampireAbility != 0)
	{
		if (vampireAbility == 1)
			bloodDropsToColor = (int)_playerSystem->player->playerTeleportSwapDamage;
		else
			if (vampireAbility == 2)
				bloodDropsToColor = (int)_playerSystem->player->playerRipPlayerDamage;
			else
				if (vampireAbility == 3)
					bloodDropsToColor = (int)_playerSystem->player->playerTeleportSwapDamage;
				else
					if (vampireAbility == 4)
						bloodDropsToColor = (int)0.0f;
	}

	for (bloodDropNumber = 0; bloodDropNumber < (playerCurrentHealth - bloodDropsToColor); bloodDropNumber++)
	{
		uiSpriteBatch->Draw(_imageElements["healthAmount"].texture.Get(), _imageElements["healthAmount"].position + bloodDropNumber * Vector2(40.0f, 0.0f), nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["healthAmount"].scale);
	}

	for (int colorBloodDropNumber = bloodDropNumber; colorBloodDropNumber < playerCurrentHealth; colorBloodDropNumber++)
	{
		uiSpriteBatch->Draw(_imageElements["healthAmountSkillCost"].texture.Get(), _imageElements["healthAmountSkillCost"].position + colorBloodDropNumber * Vector2(40.0f, 0.0f), nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["healthAmountSkillCost"].scale);
	}

	if (!vampireMode)
	{
		uiSpriteBatch->Draw(_imageElements["heroIconNormal"].texture.Get(), _imageElements["heroIconNormal"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["heroIconNormal"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKeyLPM"].texture.Get(), _imageElements["skillKeyLPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKeyLPM"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKeyPPM"].texture.Get(), _imageElements["skillKeyPPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKeyPPM"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKeyCPM"].texture.Get(), _imageElements["skillKeyCPM"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKeyCPM"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKeyE"].texture.Get(), _imageElements["skillKeyE"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKeyE"].scale);

		if (_playerSystem->cooldown->CanUseSkill("normalAttack"))
			uiSpriteBatch->Draw(_imageElements["normalAttack"].texture.Get(), _imageElements["normalAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["normalAttack"].scale);
		else
		{
			uiSpriteBatch->Draw(_imageElements["humanCoolDownFrame"].texture.Get(), _imageElements["normalAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["normalAttack"].scale);

			string value = to_string((int)_playerSystem->cooldown->RemainingCooldownTime("normalAttack"));
			wstring wide_string = wstring(value.begin(), value.end());
			const wchar_t* textValue = wide_string.c_str();

			_textElements["normalAttackCooldown"].font->DrawString(uiSpriteBatch.get(), textValue,
				_textElements["normalAttackCooldown"].position, Colors::White, 0.f, Vector2(0, 0), _textElements["normalAttackCooldown"].scale);
		}

		if (_playerSystem->cooldown->CanUseSkill("strongAttack"))
			uiSpriteBatch->Draw(_imageElements["strongAttack"].texture.Get(), _imageElements["strongAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["strongAttack"].scale);
		else
		{
			uiSpriteBatch->Draw(_imageElements["humanCoolDownFrame"].texture.Get(), _imageElements["strongAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["strongAttack"].scale);

			string value = to_string((int)_playerSystem->cooldown->RemainingCooldownTime("strongAttack"));
			wstring wide_string = wstring(value.begin(), value.end());
			const wchar_t* textValue = wide_string.c_str();

			_textElements["strongAttackCooldown"].font->DrawString(uiSpriteBatch.get(), textValue,
				_textElements["strongAttackCooldown"].position, Colors::White, 0.f, Vector2(0, 0), _textElements["strongAttackCooldown"].scale);
		}

		if (_playerSystem->cooldown->CanUseSkill("spinAttack"))
			uiSpriteBatch->Draw(_imageElements["spinAttack"].texture.Get(), _imageElements["spinAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["spinAttack"].scale);
		else
		{
			uiSpriteBatch->Draw(_imageElements["humanCoolDownFrame"].texture.Get(), _imageElements["spinAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["spinAttack"].scale);

			string value = to_string((int)_playerSystem->cooldown->RemainingCooldownTime("spinAttack"));
			wstring wide_string = wstring(value.begin(), value.end());
			const wchar_t* textValue = wide_string.c_str();

			_textElements["spinAttackCooldown"].font->DrawString(uiSpriteBatch.get(), textValue,
				_textElements["spinAttackCooldown"].position, Colors::White, 0.f, Vector2(0, 0), _textElements["spinAttackCooldown"].scale);
		}

		if (_playerSystem->cooldown->CanUseSkill("biteAttack"))
			uiSpriteBatch->Draw(_imageElements["biteAttack"].texture.Get(), _imageElements["biteAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["biteAttack"].scale);	
		else
		{
			uiSpriteBatch->Draw(_imageElements["humanCoolDownFrame"].texture.Get(), _imageElements["biteAttack"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["biteAttack"].scale);

			string value = to_string((int)_playerSystem->cooldown->RemainingCooldownTime("biteAttack"));
			wstring wide_string = wstring(value.begin(), value.end());
			const wchar_t* textValue = wide_string.c_str();

			_textElements["biteAttackCooldown"].font->DrawString(uiSpriteBatch.get(), textValue,
				_textElements["biteAttackCooldown"].position, Colors::White, 0.f, Vector2(0, 0), _textElements["biteAttackCooldown"].scale);
		}
		
	}
	else
	{
		DrawRedBorder();

		if (transitionMode)
		{
			uiSpriteBatch->Draw(_imageElements["heroIconTransitionRing"].texture.Get(), _imageElements["heroIconTransitionRing"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["heroIconTransitionRing"].scale);

			transitionElapsedTime += elapsedTime;

			if (transitionElapsedTime >= 0.5f)
			{
				transitionMode = false;
				transitionElapsedTime = 0.0f;
			}
		} 
		else
		{
			uiSpriteBatch->Draw(_imageElements["heroIconVampire"].texture.Get(), _imageElements["heroIconVampire"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["heroIconVampire"].scale);

			uiSpriteBatch->Draw(_imageElements["heroIconVampireRing"].texture.Get(), _imageElements["heroIconVampireRing"].position + Vector2(70.0f, 70.0f), nullptr, Colors::White,
				sinf(totalTime) * 6.0f, Vector2(206, 206), _imageElements["heroIconVampireRing"].scale);
		}

		uiSpriteBatch->Draw(_imageElements["skillKey1"].texture.Get(), _imageElements["skillKey1"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKey1"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKey2"].texture.Get(), _imageElements["skillKey2"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKey2"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKey3"].texture.Get(), _imageElements["skillKey3"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKey3"].scale);

		uiSpriteBatch->Draw(_imageElements["skillKey4"].texture.Get(), _imageElements["skillKey4"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["skillKey4"].scale);

		uiSpriteBatch->Draw(_imageElements["teleport"].texture.Get(), _imageElements["teleport"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["teleport"].scale);

		uiSpriteBatch->Draw(_imageElements["cleaveAttack"].texture.Get(), _imageElements["cleaveAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["cleaveAttack"].scale);

		uiSpriteBatch->Draw(_imageElements["swap"].texture.Get(), _imageElements["swap"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["swap"].scale);

		uiSpriteBatch->Draw(_imageElements["aoeAttack"].texture.Get(), _imageElements["aoeAttack"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["aoeAttack"].scale);

		if (vampireAbility != 0)
		{
			Vector2 vampireRedCirclePos = _imageElements["vamprireRedCircle"].position + Vector2(150.0f * (_playerSystem->player->vampireAbility - 1), 0.0f);

			uiSpriteBatch->Draw(_imageElements["vamprireRedCircle"].texture.Get(), vampireRedCirclePos, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["vamprireRedCircle"].scale);
		}

		uiSpriteBatch->Draw(_imageElements["teleportCost"].texture.Get(), _imageElements["teleportCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["teleportCost"].scale);

		uiSpriteBatch->Draw(_imageElements["cleaveAttackCost"].texture.Get(), _imageElements["cleaveAttackCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["cleaveAttackCost"].scale);

		uiSpriteBatch->Draw(_imageElements["swapCost"].texture.Get(), _imageElements["swapCost"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["swapCost"].scale);

	}

	uiSpriteBatch->Draw(_imageElements["fpsBackground"].texture.Get(), _imageElements["fpsBackground"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _imageElements["fpsBackground"].scale);

	fpsFont->DrawString(uiSpriteBatch.get(), fpsFontText.c_str(),
		fpsFontPos, Colors::Black, 0.f, Vector2(0, 0));

	if (menuIsOn)
	{
		uiSpriteBatch->Draw(_imageElements["popUpMenu"].texture.Get(), _imageElements["popUpMenu"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["popUpMenu"].scale);
	}

	uiSpriteBatch->End();
}

void UI::Reset()
{
	uiSpriteBatch.reset();
	uiSpriteBatchBorder.reset();
	fpsFont.reset();

	for (std::map<string, ImageUiElement>::iterator it = _imageElements.begin(); it != _imageElements.end(); ++it)
	{
		it->second.texture.Reset();
	}
}

