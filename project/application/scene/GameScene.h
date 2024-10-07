#pragma once
#include "Sprite.h"
#include "Object3d.h"
#include "Audio.h"
#include "Camera.h"
#include "ModelManager.h"

class GameScene
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

	std::vector<Sprite*> sprites;
	std::vector<Object3d*> object3ds;
	Camera* camera_ = nullptr;
	Audio::SoundData soundData;
};

