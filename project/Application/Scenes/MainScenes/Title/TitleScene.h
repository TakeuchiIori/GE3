#pragma once
// C++
#include <memory>
#include <map>

// Engine
#include "CoreScenes/Base/BaseScene.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Camera/CameraManager.h"
#include "Systems/Audio/Audio.h"
#include "Particle/ParticleEmitter.h"
#include "Object3D/Object3d.h"
#include "Player/Player.h"
#include "WorldTransform./WorldTransform.h"
#include "Drawer/LineManager/Line.h"

// Math
#include "Vector3.h"
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

