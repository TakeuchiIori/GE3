#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "ModelManager.h"

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
	Camera* camera_ = nullptr;
	// サウンド
	Audio::SoundData soundData;

	// 2Dスプライト
	std::vector<Sprite*> sprites;
	// 3Dモデル
	std::vector<Object3d*> object3ds;
};

