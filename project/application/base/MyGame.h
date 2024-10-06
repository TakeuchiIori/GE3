#pragma once
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
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
#include "Audio.h"


class MyGame
{

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了フラグのチェック
	/// </summary>
	/// <returns></returns>
	bool isEndRequst() { return endRequst_; }

private: // メンバ変数
	WinApp* winApp_ = nullptr;
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;
	// 基礎システムの初期化
	SrvManager* srvManager_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	Camera* camera_ = nullptr;
	std::vector<Sprite*> sprites;
	std::vector<Object3d*> object3ds;
	// ゲーム終了フラグ
	bool endRequst_ = false;
	Audio::SoundData soundData;
};

