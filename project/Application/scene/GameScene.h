#pragma once

// C++
#include <memory>
#include <map>

// Engine
#include "BaseScene.h"
#include "2d/sprite/Sprite.h"
#include "2d/sprite/SpriteCommon.h"
#include "bases/SrvManager.h"
#include "3d/object3d/Object3d.h"
#include "3d/object3d/Object3dCommon.h"
#include "io/audio//Audio.h"
#include "3d/light/LightManager.h"
#include "model/ModelManager.h"
#include "3d/camera/Camera.h"
#include "player/Player.h"
#include "3d/WorldTransform.h"
#include "3d/camera/CameraManager.h"
#include "io/particle/ParticleEmitter.h"
#include "3d/line/Line.h"


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
