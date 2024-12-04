#pragma once
#include <memory>
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "SrvManager.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "collider/CollisionManager.h"
#include "LightManager.h"
#include <LeakChecker.h>
// ゲーム全体
class Framework
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了フラグのチェック
	/// </summary>
	/// <returns></returns>
	virtual bool IsEndRequst() { return winApp_->ProcessMessage(); }

	// 呼び出さないとリークするぞ
	virtual ~Framework() = default;

	/// <summary>
	/// 実行
	/// </summary>
	void Run();
protected:
	// 基本的なゲームのコンポーネント
	DirectXCommon* dxCommon_;
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;

	//  ポインタ
	WinApp* winApp_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ImGuiManager* imguiManager_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	ModelManager* modelManager_ = nullptr;
	CollisionManager* collisionManager_ = nullptr;;
	LightManager* lightManager_ = nullptr;
private:
	std::unique_ptr<D3DResourceLeakChecker> leakCheck;
	// ゲーム終了フラグ
	bool endRequst_ = false;

};

