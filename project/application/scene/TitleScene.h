#pragma once
#include "BaseScene.h"
#include "sprite/Sprite.h"
#include "sprite/SpriteCommon.h"
#include "SrvManager.h"
#include "object3d/Object3d.h"
#include "object3d/Object3dCommon.h"
#include "Audio.h"
#include "ModelManager.h"
#include <memory>

class TitleScene : public BaseScene
{	
public:

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

private: // メンバ関数
	// カメラ
	std::unique_ptr<Camera> camera_;
	// サウンド
	Audio::SoundData soundData;

	// 2Dスプライト
	std::vector<std::unique_ptr<Sprite>> sprites;
	// 3Dモデル
	std::vector<std::unique_ptr<Object3d>> object3ds;
};

