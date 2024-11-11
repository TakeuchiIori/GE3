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

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

private:
    /// <summary>
    /// Catmull-Romスプラインのポイントを生成
    /// </summary>
    /// <param name="controlPoints">制御点のリスト</param>
    /// <param name="segmentCount">各セグメントにおける生成ポイント数</param>
    /// <returns>生成されたスプラインのポイントリスト</returns>
    std::vector<Vector3> GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t segmentCount);

    std::vector<Vector3> SamplePoints(const std::vector<Vector3>& points, size_t step);

    std::vector<Vector3> SampleSplineEquidistant(const std::vector<Vector3>& points, float interval);

    float interval = 5.0f; 

    std::vector<std::unique_ptr<WorldTransform>> worldTransforms_;
    std::vector<std::unique_ptr<Object3d>> obj_;
    std::vector<Vector3> controlPoints_;
    std::vector<Vector3> pointsDrawing_;
    const size_t segmentCount = 100;
};
