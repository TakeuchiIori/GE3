#pragma once
#include "BaseScene.h"
#include "sprite/Sprite.h"
#include "sprite/SpriteCommon.h"
#include "SrvManager.h"
#include "object3d/Object3d.h"
#include "object3d/Object3dCommon.h"
#include "Audio.h"
#include "light/LightManager.h"
#include "ModelManager.h"
#include "camera/Camera.h"
#include "Player.h"
#include "WorldTransform.h"
#include "camera/CameraManager.h"
#include "ParticleEmitter.h"
#include "line/Line.h"
#include <memory>
#include <map>

class TitleScene : public BaseScene
{

	enum class CameraMode
	{
		DEFAULT,
		FOLLOW,
		TOP_DOWN,
		FPS

	};

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

private:

	/// <summary>
	/// カメラモードを更新する
	/// </summary>
	void UpdateCameraMode();

	/// <summary>
	/// カメラを更新する
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// ImGui
	/// </summary>
	void ShowImGui();


private:
	// カメラ
	CameraMode cameraMode_;
	std::shared_ptr<Camera> currentCamera_;
	CameraManager cameraManager_;
	// サウンド
	Audio::SoundData soundData;
	// パーティクルエミッター
	std::unique_ptr<ParticleEmitter> particleEmitter_;
	Vector3 emitterPosition_;
	uint32_t particleCount_;

	// 3Dモデル
	std::unique_ptr<Object3d> test_;
	WorldTransform testWorldTransform_;

	// プレイヤー
	std::unique_ptr<Player> player_;

	// Line
	std::unique_ptr<Line> line_;

	Vector3 start_ = { 0.0f,0.0f,0.0f };

	Vector3 end_ = { 10.0f,0.0f,10.0f };

};

