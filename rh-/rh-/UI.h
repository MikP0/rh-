#pragma once

#include <map>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct UiElement
{
	string name;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Vector2 position;
	Vector2 scale;

	UiElement() : name("None"), position(Vector2(0.0f, 0.0f)), scale(Vector2(1.0f, 1.0f)) {};
	~UiElement() {};
};

class UI
{
public:
	UI(ID3D11Device1* device, ID3D11DeviceContext1* context, float playerHealthOrigin, shared_ptr<float> playerHealth);
	~UI();

	void Initialize();
	void DrawRedBorder();
	void Draw(bool vampireMode, bool menuIsOn);
	void Reset();


	Vector2 skillSetPosition;

	shared_ptr<DirectX::SpriteBatch> uiSpriteBatch;
	shared_ptr<DirectX::SpriteBatch> uiSpriteBatchBorder;

	shared_ptr<DirectX::SpriteFont> fpsFont;
	Vector2 fpsFontPos;
	wstring fpsFontText;


private:
	ID3D11Device1 *_device;
	ID3D11DeviceContext1 *_context;
	float _playerHealthOrigin;
	shared_ptr<float> _playerHealth;
	map<string, UiElement> _elements;
};

