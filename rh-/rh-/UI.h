#pragma once

#include <map>
#include "Cooldown.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct BasicUiElement
{
	string name;
	Vector2 position;
	Vector2 scale;

	BasicUiElement() : name("None"), position(Vector2(0.0f, 0.0f)), scale(Vector2(1.0f, 1.0f)) {};
	~BasicUiElement() {};
};

struct ImageUiElement : BasicUiElement
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	ImageUiElement() : BasicUiElement() {};
	~ImageUiElement() {};
};

struct TextUiElement : BasicUiElement
{
	shared_ptr<DirectX::SpriteFont> font;

	TextUiElement() : BasicUiElement() {};
	~TextUiElement() {};
};

class UI
{
public:
	UI(ID3D11Device1* device, ID3D11DeviceContext1* context, float playerHealthOrigin, shared_ptr<float> playerHealth, shared_ptr<Cooldown> cooldown);
	~UI();

	void Initialize();
	void DrawRedBorder();
	void Draw(bool vampireMode, int selectedVampireAbility, bool menuIsOn, float totalTime, float elapsedTime);
	void Reset();


	Vector2 skillSetPosition;

	shared_ptr<DirectX::SpriteBatch> uiSpriteBatch;
	shared_ptr<DirectX::SpriteBatch> uiSpriteBatchBorder;

	shared_ptr<DirectX::SpriteFont> fpsFont;
	Vector2 fpsFontPos;
	wstring fpsFontText;

	bool transitionMode;
	int selectedVampireAbility;

private:
	ID3D11Device1 *_device;
	ID3D11DeviceContext1 *_context;
	float _playerHealthOrigin;
	float transitionElapsedTime;
	shared_ptr<float> _playerHealth;
	map<string, ImageUiElement> _imageElements;
	map<string, TextUiElement> _textElements;
	shared_ptr<Cooldown> _cooldown;
};

