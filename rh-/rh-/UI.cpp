#include "pch.h"
#include "UI.h"

UI::UI(ID3D11Device1 * device, ID3D11DeviceContext1 * context, shared_ptr<PlayerSystem> playerSystem)
{
	_device = device;
	_context = context;
	transitionMode = false;
	messageMode = make_shared<bool>(false);
	messageToShow = 0;
	messageDelay = 0.2f;
	messageElapsedTime = 0.0f;
	transitionElapsedTime = 0.0f;
	_playerSystem = playerSystem;
	selectedVampireSkillScale = Vector2(0.03f, 0.03f);
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
		"swapCost", "aoeAttackCost","skillKeyLPM", "skillKeyPPM", "skillKeyCPM", "skillKeyE",
		"skillKey1", "skillKey2", "skillKey3", "skillKey4",
		"humanCoolDownFrame", "vampireCoolDownFrame", "humanSkillBlockade", "vampireSkillBlockade",
		"normalAttackTip", "strongAttackTip", "spinAttackTip", "biteAttackTip",
		"teleportTip", "cleaveAttackTip", "swapTip", "aoeAttackTip",
		"messageStartPlot", "messageStartTips", "messageWeapon", "messageSkills", "messageMode", "popUpMenuOptions"
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
		{"popUpMenu", "Resources\\UISprites\\ContextMenu.dds"},
		{"popUpMenuOptions", "Resources\\UISprites\\ContextMenuOptions.dds"},
		{"heroIconVampire", "Resources\\UISprites\\Hero_Circle_Vampire.dds"},
		{"heroIconVampireRing", "Resources\\UISprites\\Hero_Circle_Vampire_Ring.dds"},
		{"heroIconTransitionRing", "Resources\\UISprites\\Hero_Circle_Transition.dds"},
		{"vamprireRedCircle", "Resources\\UISprites\\Vampire_Red_Circle.dds"},
		{"teleportCost", "Resources\\UISprites\\Blood_Drop_One.dds"},
		{"cleaveAttackCost", "Resources\\UISprites\\Blood_Drop_Two.dds"},
		{"swapCost", "Resources\\UISprites\\Blood_Drop_One.dds"},
		{"aoeAttackCost", "Resources\\UISprites\\Blood_Drop_Three.dds"},
		{"skillKeyLPM", "Resources\\UISprites\\Human_Skill_Key_LPM.dds"},
		{"skillKeyPPM", "Resources\\UISprites\\Human_Skill_Key_PPM.dds"},
		{"skillKeyCPM", "Resources\\UISprites\\Human_Skill_Key_CPM.dds"},
		{"skillKeyE", "Resources\\UISprites\\Human_Skill_Key_E.dds"},
		{"skillKey1", "Resources\\UISprites\\Vampire_Skill_Key_1.dds"},
		{"skillKey2", "Resources\\UISprites\\Vampire_Skill_Key_2.dds"},
		{"skillKey3", "Resources\\UISprites\\Vampire_Skill_Key_3.dds"},
		{"skillKey4", "Resources\\UISprites\\Vampire_Skill_Key_4.dds"},
		{"humanCoolDownFrame", "Resources\\UISprites\\Human_Skill_Cooldown_Frame.dds"},
		{"vampireCoolDownFrame", "Resources\\UISprites\\Vampire_Skill_Cooldown_Frame.dds"},
		{"humanSkillBlockade", "Resources\\UISprites\\Human_Blocked_Skill.dds"},
		{"vampireSkillBlockade", "Resources\\UISprites\\Vampire_Blocked_Skill.dds"},
		{"normalAttackTip", "Resources\\UISprites\\Normal_Attack_Tip.dds"},
		{"strongAttackTip", "Resources\\UISprites\\Strong_Attack_Tip.dds"},
		{"spinAttackTip", "Resources\\UISprites\\Spin_Attack_Tip.dds"},
		{"biteAttackTip", "Resources\\UISprites\\Bite_Attack_Tip.dds"},
		{"teleportTip", "Resources\\UISprites\\Teleport_Tip.dds"},
		{"cleaveAttackTip", "Resources\\UISprites\\Cleave_Attack_Tip.dds"},
		{"swapTip", "Resources\\UISprites\\Swap_Tip.dds"},
		{"aoeAttackTip", "Resources\\UISprites\\Aoe_Attack_Tip.dds"},
		{"messageStartPlot", "Resources\\UISprites\\Message_Start_Plot.dds"},
		{"messageStartTips", "Resources\\UISprites\\Message_Start_Tips.dds"},
		{"messageWeapon", "Resources\\UISprites\\Message_Weapon.dds"},
		{"messageSkills", "Resources\\UISprites\\Message_Skills.dds"},
		{"messageMode", "Resources\\UISprites\\Message_Mode.dds"}
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
		{"popUpMenu", Vector2(750.0f, 180.0f)},
		{"popUpMenuOptions", Vector2(750.0f, 180.0f)},
		{"heroIconVampire", Vector2(0.0f, 0.0f)},
		{"heroIconVampireRing", Vector2(0.0f, 0.0f)},
		{"heroIconTransitionRing", Vector2(0.0f, 0.0f)},
		{"vamprireRedCircle", skillSetPosition + Vector2(-6.0f, -12.0f)},
		{"teleportCost", skillSetPosition + Vector2(25.0f, -60.0f)},
		{"cleaveAttackCost", Vector2(skillSetPosition.x + 160.0f, skillSetPosition.y - 60.0f)},
		{"swapCost", Vector2(skillSetPosition.x + 325.0f, skillSetPosition.y - 60.0f)},
		{"aoeAttackCost", Vector2(skillSetPosition.x + 440.0f, skillSetPosition.y - 60.0f)},
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
		{"biteAttackCooldown", skillSetPosition + Vector2(466.0f, 5.0f)},
		{"humanSkillBlockade", skillSetPosition + Vector2(5.0f, 0.0f)},
		{"vampireSkillBlockade", skillSetPosition + Vector2(-5.0f, -10.0f)},
		{"normalAttackTip", skillSetPosition + Vector2(-70.0f, -230.0f)},
		{"strongAttackTip", skillSetPosition + Vector2(85.0f, -230.0f)},
		{"spinAttackTip", skillSetPosition + Vector2(240.0f, -230.0f)},
		{"biteAttackTip", skillSetPosition + Vector2(395.0f, -230.0f)},
		{"teleportTip", skillSetPosition + Vector2(-70.0f, -230.0f)},
		{"cleaveAttackTip", skillSetPosition + Vector2(85.0f, -230.0f)},
		{"swapTip", skillSetPosition + Vector2(240.0f, -230.0f)},
		{"aoeAttackTip", skillSetPosition + Vector2(395.0f, -230.0f)},
		{"messageStartPlot", Vector2(500.0f, 250.0f)},
		{"messageStartTips", Vector2(500.0f, 250.0f)},
		{"messageWeapon", Vector2(500.0f, 250.0f)},
		{"messageSkills", Vector2(500.0f, 250.0f)},
		{"messageMode", Vector2(500.0f, 250.0f)}
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
		{"popUpMenu", Vector2(0.8f, 0.8f)},
		{"popUpMenuOptions", Vector2(0.8f, 0.8f)},
		{"heroIconVampire", Vector2(0.35f, 0.35f)},
		{"heroIconVampireRing", Vector2(0.35f, 0.35f)},
		{"heroIconTransitionRing", Vector2(0.35f, 0.35f)},
		{"vamprireRedCircle", Vector2(0.25f, 0.25f)},
		{"teleportCost", Vector2(0.25f, 0.25f)},
		{"cleaveAttackCost", Vector2(0.25f, 0.25f)},
		{"swapCost", Vector2(0.25f, 0.25f)},
		{"aoeAttackCost", Vector2(0.25f, 0.25f)},
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
		{"biteAttackCooldown", Vector2(1.6f, 1.6f)},
		{"humanSkillBlockade", Vector2(0.20f, 0.20f)},
		{"vampireSkillBlockade", Vector2(0.24f, 0.24f)},
		{"normalAttackTip", Vector2(0.50f, 0.50f)},
		{"strongAttackTip", Vector2(0.50f, 0.50f)},
		{"spinAttackTip", Vector2(0.50f, 0.50f)},
		{"biteAttackTip", Vector2(0.50f, 0.50f)},
		{"teleportTip", Vector2(0.50f, 0.50f)},
		{"cleaveAttackTip", Vector2(0.50f, 0.50f)},
		{"swapTip", Vector2(0.50f, 0.50f)},
		{"aoeAttackTip", Vector2(0.50f, 0.50f)},
		{"messageStartPlot", Vector2(0.40f, 0.40f)},
		{"messageStartTips", Vector2(0.40f, 0.40f)},
		{"messageWeapon", Vector2(0.40f, 0.40f)},
		{"messageSkills", Vector2(0.40f, 0.40f)},
		{"messageMode", Vector2(0.40f, 0.40f)}
	};


	for each(string uiElement in uiImageNames)
	{
		_imageElements[uiElement] = ImageUiElement();
		_imageElements[uiElement].name = uiElement;

		wstring wide_string = wstring(uiImageNameTexMap[uiElement].begin(), uiImageNameTexMap[uiElement].end());
		const wchar_t* spritePath = wide_string.c_str();
		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(_device, spritePath,
				_imageElements[uiElement].resource.GetAddressOf(),
				_imageElements[uiElement].texture.ReleaseAndGetAddressOf()));

		_imageElements[uiElement].position = uiNamePositionMap[uiElement];
		_imageElements[uiElement].scale = uiNameScaleMap[uiElement];

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
		DX::ThrowIfFailed(_imageElements[uiElement].resource.As(&texture2D));
		CD3D11_TEXTURE2D_DESC texture2Ddesc;
		texture2D->GetDesc(&texture2Ddesc);

		_imageElements[uiElement].endPos = _imageElements[uiElement].position + _imageElements[uiElement].scale * Vector2(texture2Ddesc.Width, texture2Ddesc.Height);
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
	uiSpriteBatchMessages = std::make_shared<SpriteBatch>(_context);

	fpsFont = std::make_unique<SpriteFont>(_device, L"Resources\\Fonts\\fpsFont.spritefont");
	fpsFontPos = Vector2(710.0f, 10.0f);
	fpsFontText = std::wstring(L"60");

}

void UI::CheckSkillTips(bool vampireMode)
{
	Mouse::State mouse = Input::GetMouseState();

	vector<string> skillsNames = {
			"normalAttack", "strongAttack", "spinAttack", "biteAttack",
			"teleport", "cleaveAttack", "swap", "aoeAttack"
	};

	if (!vampireMode)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mouse.x >= _imageElements[skillsNames[i]].position.x &&
				mouse.y >= _imageElements[skillsNames[i]].position.y &&
				mouse.x <= _imageElements[skillsNames[i]].endPos.x &&
				mouse.y <= _imageElements[skillsNames[i]].endPos.y)
			{
				uiSpriteBatch->Draw(_imageElements[skillsNames[i] + "Tip"].texture.Get(), _imageElements[skillsNames[i] + "Tip"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements[skillsNames[i] + "Tip"].scale);
			}
		}
	}
	else
	{
		for (int i = 4; i < 8; i++)
		{
			if (mouse.x >= _imageElements[skillsNames[i]].position.x &&
				mouse.y >= _imageElements[skillsNames[i]].position.y &&
				mouse.x <= _imageElements[skillsNames[i]].endPos.x &&
				mouse.y <= _imageElements[skillsNames[i]].endPos.y)
			{
				uiSpriteBatch->Draw(_imageElements[skillsNames[i] + "Tip"].texture.Get(), _imageElements[skillsNames[i] + "Tip"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements[skillsNames[i] + "Tip"].scale);
			}
		}
	}
}

void UI::ShowMessages(float elapsedTime)
{
	Mouse::State mouse = Input::GetMouseState();
	Keyboard::State keyboard = Input::GetKeyboardState();

	uiSpriteBatchMessages->Begin();

	switch (messageToShow)
	{
		case 1:
		{
			uiSpriteBatchMessages->Draw(_imageElements["messageStartPlot"].texture.Get(), _imageElements["messageStartPlot"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["messageStartPlot"].scale);

			if (keyboard.Enter && messageElapsedTime > messageDelay)
			{
				messageToShow = 2;
				messageElapsedTime = 0.0f;
			}
		} break;

		case 2:
		{
			uiSpriteBatchMessages->Draw(_imageElements["messageStartTips"].texture.Get(), _imageElements["messageStartTips"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["messageStartTips"].scale);

			if (keyboard.Enter && messageElapsedTime > messageDelay)
			{
				messageToShow = 0;
				messageElapsedTime = 0.0f;
			}
		} break;

		case 3:
		{
			if (messageElapsedTime > 4.0f)
				uiSpriteBatchMessages->Draw(_imageElements["messageWeapon"].texture.Get(), _imageElements["messageWeapon"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["messageWeapon"].scale);

			if (keyboard.Enter && messageElapsedTime > (4.0f + messageDelay))
			{
				messageToShow = 4;
				messageElapsedTime = 0.0f;
			}
		} break;

		case 4:
		{
			uiSpriteBatchMessages->Draw(_imageElements["messageSkills"].texture.Get(), _imageElements["messageSkills"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["messageSkills"].scale);

			if (keyboard.Enter && messageElapsedTime > messageDelay)
			{
				messageToShow = 5;
				messageElapsedTime = 0.0f;
			}
		} break;

		case 5:
		{
			uiSpriteBatchMessages->Draw(_imageElements["messageMode"].texture.Get(), _imageElements["messageMode"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["messageMode"].scale);

			if (keyboard.Enter && messageElapsedTime > messageDelay)
			{
				messageToShow = 0;
				messageElapsedTime = 0.0f;
			}
		} break;
	}

	messageElapsedTime += elapsedTime;

	uiSpriteBatchMessages->End();
}

void UI::DrawRedBorder()
{
	uiSpriteBatchBorder->Begin();

	uiSpriteBatchBorder->Draw(_imageElements["vampireModeBorder"].texture.Get(), _imageElements["vampireModeBorder"].position, nullptr, Colors::White,
		0.f, Vector2(0, 0), _imageElements["vampireModeBorder"].scale);

	uiSpriteBatchBorder->End();
}

void UI::Draw(int menuIsOn, float totalTime, float elapsedTime, bool humanMode)
{
	bool vampireMode = _playerSystem->vampireMode;
	int vampireAbility = vampireMode != 0 ? _playerSystem->player->vampireAbility : 0;
	int bloodDropsToColor = 0;
	int bloodDropNumber;
	int playerCurrentHealth = (int)*_playerSystem->playerHealth;

	uiSpriteBatch->Begin();

	if (!humanMode)
	{
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
							bloodDropsToColor = (int)_playerSystem->player->playerAOEDamage;
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

			if (!_playerSystem->blockade->IsSkillBlocked("normalAttack"))
			{
				uiSpriteBatch->Draw(_imageElements["skillKeyLPM"].texture.Get(), _imageElements["skillKeyLPM"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["skillKeyLPM"].scale);

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
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["humanSkillBlockade"].texture.Get(), _imageElements["normalAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["normalAttack"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("strongAttack"))
			{
				uiSpriteBatch->Draw(_imageElements["skillKeyPPM"].texture.Get(), _imageElements["skillKeyPPM"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["skillKeyPPM"].scale);

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
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["humanSkillBlockade"].texture.Get(), _imageElements["strongAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["strongAttack"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("spinAttack"))
			{
				uiSpriteBatch->Draw(_imageElements["skillKeyCPM"].texture.Get(), _imageElements["skillKeyCPM"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["skillKeyCPM"].scale);

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
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["humanSkillBlockade"].texture.Get(), _imageElements["spinAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["spinAttack"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("biteAttack"))
			{
				uiSpriteBatch->Draw(_imageElements["skillKeyE"].texture.Get(), _imageElements["skillKeyE"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["skillKeyE"].scale);

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
				uiSpriteBatch->Draw(_imageElements["humanSkillBlockade"].texture.Get(), _imageElements["biteAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["biteAttack"].scale);
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

			if (!_playerSystem->blockade->IsSkillBlocked("teleport"))
			{
				if (vampireAbility == 1)
				{
					uiSpriteBatch->Draw(_imageElements["skillKey1"].texture.Get(), _imageElements["skillKey1"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey1"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["teleport"].texture.Get(), _imageElements["teleport"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["teleport"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["teleportCost"].texture.Get(), _imageElements["teleportCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["teleportCost"].scale + selectedVampireSkillScale);
				}
				else
				{
					uiSpriteBatch->Draw(_imageElements["skillKey1"].texture.Get(), _imageElements["skillKey1"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey1"].scale);

					uiSpriteBatch->Draw(_imageElements["teleport"].texture.Get(), _imageElements["teleport"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["teleport"].scale);

					uiSpriteBatch->Draw(_imageElements["teleportCost"].texture.Get(), _imageElements["teleportCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["teleportCost"].scale);
				}		
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["vampireSkillBlockade"].texture.Get(), _imageElements["teleport"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["teleport"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("cleaveAttack"))
			{
				if (vampireAbility == 2)
				{
					uiSpriteBatch->Draw(_imageElements["skillKey2"].texture.Get(), _imageElements["skillKey2"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey2"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["cleaveAttack"].texture.Get(), _imageElements["cleaveAttack"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["cleaveAttack"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["cleaveAttackCost"].texture.Get(), _imageElements["cleaveAttackCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["cleaveAttackCost"].scale + selectedVampireSkillScale);
				}
				else
				{
					uiSpriteBatch->Draw(_imageElements["skillKey2"].texture.Get(), _imageElements["skillKey2"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey2"].scale);

					uiSpriteBatch->Draw(_imageElements["cleaveAttack"].texture.Get(), _imageElements["cleaveAttack"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["cleaveAttack"].scale);

					uiSpriteBatch->Draw(_imageElements["cleaveAttackCost"].texture.Get(), _imageElements["cleaveAttackCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["cleaveAttackCost"].scale);
				}
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["vampireSkillBlockade"].texture.Get(), _imageElements["cleaveAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["cleaveAttack"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("swap"))
			{
				if (vampireAbility == 3)
				{
					uiSpriteBatch->Draw(_imageElements["skillKey3"].texture.Get(), _imageElements["skillKey3"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey3"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["swap"].texture.Get(), _imageElements["swap"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["swap"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["swapCost"].texture.Get(), _imageElements["swapCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["swapCost"].scale + selectedVampireSkillScale);
				}
				else
				{
					uiSpriteBatch->Draw(_imageElements["skillKey3"].texture.Get(), _imageElements["skillKey3"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey3"].scale);

					uiSpriteBatch->Draw(_imageElements["swap"].texture.Get(), _imageElements["swap"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["swap"].scale);

					uiSpriteBatch->Draw(_imageElements["swapCost"].texture.Get(), _imageElements["swapCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["swapCost"].scale);
				}	
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["vampireSkillBlockade"].texture.Get(), _imageElements["swap"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["swap"].scale);
			}

			if (!_playerSystem->blockade->IsSkillBlocked("aoeAttack"))
			{
				if (vampireAbility == 4)
				{
					uiSpriteBatch->Draw(_imageElements["skillKey4"].texture.Get(), _imageElements["skillKey4"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey4"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["aoeAttack"].texture.Get(), _imageElements["aoeAttack"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["aoeAttack"].scale + selectedVampireSkillScale);

					uiSpriteBatch->Draw(_imageElements["aoeAttackCost"].texture.Get(), _imageElements["aoeAttackCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["aoeAttackCost"].scale + selectedVampireSkillScale);
				}
				else
				{
					uiSpriteBatch->Draw(_imageElements["skillKey4"].texture.Get(), _imageElements["skillKey4"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["skillKey4"].scale);

					uiSpriteBatch->Draw(_imageElements["aoeAttack"].texture.Get(), _imageElements["aoeAttack"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["aoeAttack"].scale);

					uiSpriteBatch->Draw(_imageElements["aoeAttackCost"].texture.Get(), _imageElements["aoeAttackCost"].position, nullptr, Colors::White,
						0.f, Vector2(0, 0), _imageElements["aoeAttackCost"].scale);
				}
			}
			else
			{
				uiSpriteBatch->Draw(_imageElements["vampireSkillBlockade"].texture.Get(), _imageElements["aoeAttack"].position, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["aoeAttack"].scale);
			}

			if (vampireAbility != 0)
			{
				Vector2 vampireRedCirclePos = _imageElements["vamprireRedCircle"].position + Vector2(150.0f * (vampireAbility - 1), 0.0f);

				uiSpriteBatch->Draw(_imageElements["vamprireRedCircle"].texture.Get(), vampireRedCirclePos, nullptr, Colors::White,
					0.f, Vector2(0, 0), _imageElements["vamprireRedCircle"].scale + selectedVampireSkillScale);
			}
		}

		CheckSkillTips(vampireMode);

		uiSpriteBatch->Draw(_imageElements["fpsBackground"].texture.Get(), _imageElements["fpsBackground"].position, nullptr, Colors::White,
			0.f, Vector2(0, 0), _imageElements["fpsBackground"].scale);

		fpsFont->DrawString(uiSpriteBatch.get(), fpsFontText.c_str(),
			fpsFontPos, Colors::Black, 0.f, Vector2(0, 0));

		if (menuIsOn == 1)
		{
			uiSpriteBatch->Draw(_imageElements["popUpMenu"].texture.Get(), _imageElements["popUpMenu"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["popUpMenu"].scale);
		}
		else if (menuIsOn == 2)
		{
			uiSpriteBatch->Draw(_imageElements["popUpMenuOptions"].texture.Get(), _imageElements["popUpMenuOptions"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["popUpMenuOptions"].scale);
		}
	}
	else
	{
		if (menuIsOn == 1)
		{
			uiSpriteBatch->Draw(_imageElements["popUpMenu"].texture.Get(), _imageElements["popUpMenu"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["popUpMenu"].scale);
		}
		else if (menuIsOn == 2)
		{
			uiSpriteBatch->Draw(_imageElements["popUpMenuOptions"].texture.Get(), _imageElements["popUpMenuOptions"].position, nullptr, Colors::White,
				0.f, Vector2(0, 0), _imageElements["popUpMenuOptions"].scale);
		}
	}
	if (messageToShow != 0)
	{
		*messageMode = true;
		ShowMessages(elapsedTime);
	}
	else
		*messageMode = false;


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

