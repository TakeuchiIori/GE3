#pragma once
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "Model.h"
#include "ModelManager.h"
#include "imgui_impl_win32.h"
#include "Camera.h"
#include "SrvManager.h"
#include "Framework.h"

class MyGame : public Framework
{

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private: // メンバ変数
	Camera* camera_ = nullptr;
	std::vector<Sprite*> sprites;
	std::vector<Object3d*> object3ds;
	Audio::SoundData soundData;
};

