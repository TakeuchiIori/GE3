#pragma once

// Game
#include "BaseScene.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Player.h"
#include "WorldTransform.h"
#include "CameraManager.h"
#include "Spline.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "RailCamera.h"

// C++
#include <memory>
enum class CameraMode
{
    FOLLOW,
    TOP_DOWN,
    FPS,
    DEBUG
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
    /// 衝突判定を応答
    /// </summary>
    void CheackAllCollisions();


    /// <summary>
    /// カメラを解放する
    /// </summary>
    void FinalizeCamera();

    /// <summary>
    /// カメラモードを更新する
    /// </summary>
    void UpdateCameraMode();

    /// <summary>
    /// カメラを更新する
    /// </summary>
    void UpdateCamera();

    /// <summary>
    /// 右スティックの入力に基づきカメラを操作し、プレイヤーの向きも同期させる
    /// </summary>
    void UpdateCameraWithRightStick();

    /// <summary>
    /// 描画の準備をする
    /// </summary>
    void PrepareDraw();

private: // メンバ変数
    CameraMode cameraMode_;
    std::shared_ptr<Camera> currentCamera_;
    CameraManager cameraManager_;
    Audio::SoundData soundData;

    // 3Dモデル
    std::unique_ptr<Object3d> test_;
    WorldTransform testWorldTransform_;

    std::unique_ptr<Player> player_;
    std::unique_ptr<Spline> spline_;
    std::unique_ptr<RailCamera> railCamera_;

    // まだ複数化していない
    std::list<Enemy*> enemies_;
    std::list<EnemyBullet*> enemyBullets_;

    size_t splineIndex_;  // スプライン上の位置を管理するインデックス
};
