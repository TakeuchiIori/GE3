#pragma once
// C++
#include <memory>

// Engine
#include "bases/WinApp.h"
#include "bases/ImGuiManager.h"
#include "bases/SrvManager.h"
#include "bases/DirectXCommon.h"
#include "bases/TextureManager.h"
#include "2D/sprite/SpriteCommon.h"
#include "3D/SceneManager.h"
#include "3d/object3d/Object3dCommon.h"
#include "3D/collider/CollisionManager.h"
#include "3D/light/LightManager.h"
#include "3D/line/LineManager.h"
#include "model/ModelManager.h"
#include "io/input/Input.h"
#include "io/audio/Audio.h"
#include "utility/AbstractSceneFactory.h"
#include "utility/LeakChecker.h"

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
	LineManager* lineManager_ = nullptr;
private:
	
	// ゲーム終了フラグ
	bool endRequst_ = false;

};

