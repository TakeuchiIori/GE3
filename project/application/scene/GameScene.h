#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Audio.h"
#include "ModelManager.h"
#include "Camera.h"
#include <memory>
#include "Player.h"
#include "WorldTransform.h"
#include "CameraManager.h"
#include "Spline.h"
enum class CameraMode
{
    FOLLOW,
    TOP_DOWN
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
    /// モデルを読み込む
    /// </summary>
    void LoadModels();

    /// <summary>
    /// カメラを初期化する
    /// </summary>
    void InitializeCamera();

    /// <summary>
    /// カメラを解放する
    /// </summary>
    void FinalizeCamera();

    /// <summary>
    /// オブジェクトを初期化する
    /// </summary>
    void InitializeObjects();

    /// <summary>
    /// プレイヤーを更新する
    /// </summary>
    void UpdatePlayer();

    /// <summary>
    /// カメラモードを更新する
    /// </summary>
    void UpdateCameraMode();

    /// <summary>
    /// カメラを更新する
    /// </summary>
    void UpdateCamera();

    /// <summary>
    /// 描画の準備をする
    /// </summary>
    void PrepareDraw();

    /// <summary>
    /// オブジェクトを描画する
    /// </summary>
    void DrawObjects();

private: // メンバ変数
    CameraMode cameraMode_;
    std::shared_ptr<Camera> currentCamera_;
    CameraManager cameraManager_;
    Audio::SoundData soundData;

    // 3Dモデル
    std::unique_ptr<Object3d> test_;
    WorldTransform testWorldTransform_;

    std::unique_ptr<Player> player_;
    std::unique_ptr< Spline> spline_;
};
