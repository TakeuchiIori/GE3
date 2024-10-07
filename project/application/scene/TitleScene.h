#pragma once
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "ModelManager.h"

class TitleScene
{	
public:

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

