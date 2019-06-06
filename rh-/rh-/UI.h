#pragma once

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class UI
{
public:
	UI(ID3D11Device1* device, ID3D11DeviceContext1* context, float playerHealthOrigin, shared_ptr<float> playerHealth);
	~UI();

	void Initialize();
	void DrawRedBorder();
	void Draw(bool vampireMode, bool menuIsOn);
	void Reset();

	//UI Sprites
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarHeroTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthBarHealthTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> redBorderTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> powerAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> spinAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> biteAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> swapAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> aoeAttackTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dashTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> heroCircleTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ripTex;

	Vector2 healthBarPos;
	Vector2 healthBarHeroPos;
	Vector2 healthBarHealthPos;
	Vector2 healthBarHealthScale;
	Vector2 redBorderPos;
	Vector2 redBorderScale;
	Vector2 normalAttackPos;
	Vector2 normalAttackScale;
	Vector2 powerAttackPos;
	Vector2 powerAttackScale;
	Vector2 spinAttackPos;
	Vector2 spinAttackScale;
	Vector2 biteAttackPos;
	Vector2 biteAttackScale;
	Vector2 swapAttackPos;
	Vector2 swapAttackScale;
	Vector2 aoeAttackPos;
	Vector2 aoeAttackScale;
	Vector2 dashPos;
	Vector2 dashScale;
	Vector2 heroCircleAttackPos;
	Vector2 heroCircleAttackScale;
	Vector2 ripAttackPos;
	Vector2 ripAttackScale;
	Vector2 skillSetPosition;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fpsBarTex;
	Vector2 fpsBarPos;

	shared_ptr<DirectX::SpriteBatch> uiSpriteBatch;
	shared_ptr<DirectX::SpriteBatch> uiSpriteBatchBorder;

	shared_ptr<DirectX::SpriteFont> fpsFont;
	Vector2 fpsFontPos;
	wstring fpsFontText;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> menuTex;
	Vector2 menuPos;


private:
	ID3D11Device1 *_device;
	ID3D11DeviceContext1 *_context;
	float _playerHealthOrigin;
	shared_ptr<float> _playerHealth;
};

