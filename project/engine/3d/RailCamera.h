#pragma once
#include "Spline.h"
#include "WorldTransform.h"
#include <vector>

/// <summary>
/// スプラインに沿って移動するレールカメラ
/// </summary>
class RailCamera {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    RailCamera();

    /// <summary>
    /// スプラインのパスを設定する
    /// </summary>
    void SetSplinePath(const std::vector<Vector3>& splinePoints);

    /// <summary>
    /// 更新処理：スプラインに沿ってカメラを移動
    /// </summary>
    void Update();

    /// <summary>
    /// 親トランスフォームを設定する
    /// </summary>
    void SetParent(const WorldTransform* parent);

    /// <summary>
    /// カメラのワールドトランスフォームを取得
    /// </summary>
    const WorldTransform& GetWorldTransform() const{ return worldTransform_; }

    void SetSpeed(float speed) { speed_ = speed; }

private:
    WorldTransform worldTransform_;
    const std::vector<Vector3>* splinePoints_ = nullptr;
    float speed_ = 0.1f;  // デフォルトの移動速度
    float currentPosition_ = 0.0f;                    // 現在のスプライン上のインデックス
};
