#pragma once

#include <map>
#include "PlayerSystem.h"

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
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Vector2 endPos;

	ImageUiElement() : BasicUiElement(), endPos(Vector2(0.0f, 0.0f)) {};
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
	UI(ID3D11Device1* device, ID3D11DeviceContext1* context, shared_ptr<PlayerSystem> playerSystem);
	~UI();

	void Initialize();
	void CheckSkillTips(bool vampireMode);
	void ShowMessages(float elapsedTime);
	void DrawRedBorder();
	void Draw(int menuIsOn, float totalTime, float elapsedTime, bool humanMode);
	void Reset();


	Vector2 skillSetPosition;
	Vector2 selectedVampireSkillScale;

	shared_ptr<DirectX::SpriteBatch> uiSpriteBatch;
	shared_ptr<DirectX::SpriteBatch> uiSpriteBatchBorder;
	shared_ptr<DirectX::SpriteBatch> uiSpriteBatchMessages;

	shared_ptr<DirectX::SpriteFont> fpsFont;
	Vector2 fpsFontPos;
	wstring fpsFontText;

	bool transitionMode;
	shared_ptr<bool> messageMode;
	int messageToShow;
	float messageDelay;
	float messageElapsedTime;

private:
	ID3D11Device1 *_device;
	ID3D11DeviceContext1 *_context;
	float transitionElapsedTime;
	map<string, ImageUiElement> _imageElements;
	map<string, TextUiElement> _textElements;
	shared_ptr<PlayerSystem> _playerSystem;
};

