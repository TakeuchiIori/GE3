#pragma once
#include "MathFunc.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include <vector>
#include <memory>

/// <summary>
/// スプラインの描画と更新を行うクラス
/// </summary>
class Spline
{
public:

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    std::vector<Vector3> GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t index, size_t segmentCount);

    std::vector<Vector3> SampleSplineEquidistant(const std::vector<Vector3>& points, size_t sampleCount);



    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

private:
    std::vector<Vector3> controlPoints_; // 制御点
    std::vector<size_t> segmentCounts_;  // 各セグメントの分割数
    std::vector<Vector3> pointsDrawing_; // 描画用のスプラインポイント

    std::vector<std::unique_ptr<Object3d>> obj_; // 各ポイントに対応するObject3d
    std::vector<std::unique_ptr<WorldTransform>> worldTransforms_; // 各ポイントのワールド変換

    Quaternion lastRotation_; // 最後の回転を保持するメンバ変数
    const size_t sampleCount = 200; // サンプル数を設定（任意の数に設定）

   
};


