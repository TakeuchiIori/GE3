#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "LightManager.h"
#include "ModelManager.h"
#include "Camera.h"
#include <memory>
#include "Player.h"
#include "WorldTransform.h"
#include "CameraManager.h"
#include "ParticleEmitter.h"
#include "Enemy.h"

enum class CameraMode
{
    DEFAULT,
    FOLLOW,
    TOP_DOWN,
    FPS

};

class GameScene : public BaseScene
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

private:

    /// <summary>
    /// カメラモードを更新する
    /// </summary>
    void UpdateCameraMode();

    /// <summary>
    /// カメラを更新する
    /// </summary>
    void UpdateCamera();


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
    Transform t;
    // 3Dモデル
    std::unique_ptr<Object3d> test_;
    WorldTransform testWorldTransform_;

    // プレイヤー
    std::unique_ptr<Player> player_;

    std::unique_ptr<Enemy> enemy_;
};
