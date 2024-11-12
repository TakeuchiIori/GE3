#include "Spline.h"
#include "ModelManager.h"
#include <vector>
#include <memory>
#include "Vector3.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <MathFunc.h>

#include "Spline.h"
#include "ModelManager.h"
#include <vector>
#include <memory>
#include "Vector3.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <MathFunc.h>

#include "Spline.h"
#include "ModelManager.h"
#include <vector>
#include <memory>
#include "Vector3.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <MathFunc.h>

/// <summary>
/// 初期化
/// </summary>
void Spline::Initialize()
{
    obj_.clear();
    worldTransforms_.clear();

    // .objモデルの読み込み
    ModelManager::GetInstance()->LoadModel("rail.obj");

    // 制御点の初期化
    controlPoints_ = {
        {0, 0, 0}, {30, 10, 10}, {60, 20, 15}, {90, 25, 20},
        {120, 20, 15}, {150, 10, 5}, {180, 15, -5}, {210, 20, -10},
        {240, 15, -5}, {270, 10, 5}, {300, 0, 10}, {330, -10, 5}
    };
    segmentCounts_ = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 }; // 分割数設定

    // 制御点間でCatmull-Romスプラインを使用して滑らかなスプラインポイントを生成
    pointsDrawing_.clear();
    for (size_t i = 0; i < controlPoints_.size() - 1; ++i) {
        auto splinePoints = GenerateCatmullRomSplinePoints(controlPoints_, i, segmentCounts_[i]);
        pointsDrawing_.insert(pointsDrawing_.end(), splinePoints.begin(), splinePoints.end());
    }

    // 各スプラインポイントに対応するオブジェクトとワールド変換を生成
    for (size_t i = 0; i < pointsDrawing_.size(); ++i) {
        auto obj = std::make_unique<Object3d>();
        obj->Initialize();
        obj->SetModel("rail.obj");
        obj_.push_back(std::move(obj));

        auto worldTransform = std::make_unique<WorldTransform>();
        worldTransform->Initialize();
        worldTransform->translation_ = pointsDrawing_[i];

        // 回転の設定：スプラインの方向に沿わせる
        if (i > 0) {
            Vector3 currentDirection = Normalize(pointsDrawing_[i] - pointsDrawing_[i - 1]);
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(0.0f, 0.0f, 1.0f), currentDirection); // Z軸を基準方向に変更
            worldTransform->rotation_ = QuaternionToEuler(rotationQuat);
        }
        else if (pointsDrawing_.size() > 1) {
            // 最初のレールに対しても2番目のポイントとの方向を基に回転を設定
            Vector3 initialDirection = Normalize(pointsDrawing_[1] - pointsDrawing_[0]);
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(0.0f, 0.0f, 1.0f), initialDirection); // Z軸を基準方向に変更
            worldTransform->rotation_ = QuaternionToEuler(rotationQuat);
        }

        // スケールの設定
        if (i > 0) {
            float segmentLength = Length(pointsDrawing_[i] - pointsDrawing_[i - 1]);

            // 分割数に応じた縦（Y）スケールと横（X）スケールの自動調整
            int segmentCount = segmentCounts_[(std::min)(i, segmentCounts_.size() - 1)];
            float xScaleFactor = segmentLength / static_cast<float>(segmentCount); // Xスケール：セグメント間の距離に基づく
            float yScaleFactor = 10.0f / static_cast<float>(segmentCount);         // Yスケール：分割数が増えると小さく、減ると大きくなる

            worldTransform->scale_ = Vector3(1.0f, xScaleFactor, 1.0f);
        }
        else {
            // 最初のポイントはデフォルトスケール
            worldTransform->scale_ = Vector3(1.0f, 1.0f, 1.0f);
        }

        worldTransforms_.push_back(std::move(worldTransform));
    }
}

/// <summary>
/// 更新処理
/// </summary>
void Spline::Update()
{
    for (size_t i = 0; i < pointsDrawing_.size(); ++i)
    {
        // 各ポイントの位置を更新
        worldTransforms_[i]->translation_ = pointsDrawing_[i];

        if (i > 0) {
            // 前のポイントと現在のポイントを使用して方向と回転を計算
            Vector3 startPoint = pointsDrawing_[i - 1];
            Vector3 endPoint = pointsDrawing_[i];
            Vector3 direction = Normalize(endPoint - startPoint);

            // クォータニオンで回転を設定：スプライン曲線の方向に合わせる
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(0.0f, 0.0f, 1.0f), direction); // Z軸を基準方向に変更
            worldTransforms_[i]->rotation_ = QuaternionToEuler(rotationQuat);
        }
        else if (pointsDrawing_.size() > 1) {
            // 最初のレールに回転を設定
            Vector3 initialDirection = Normalize(pointsDrawing_[1] - pointsDrawing_[0]);
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(0.0f, 0.0f, 1.0f), initialDirection); // Z軸を基準方向に変更
            worldTransforms_[i]->rotation_ = QuaternionToEuler(rotationQuat);
        }

        // スケールの設定
        if (i > 0) {
            float segmentLength = Length(pointsDrawing_[i] - pointsDrawing_[i - 1]);

            // 分割数に応じた縦（Y）スケールと横（X）スケールの自動調整
            int segmentCount = segmentCounts_[(std::min)(i, segmentCounts_.size() - 1)];
            float xScaleFactor = segmentLength / static_cast<float>(segmentCount); // Xスケール：セグメント間の距離に基づく
            float yScaleFactor = 10.0f / static_cast<float>(segmentCount);         // Yスケール：分割数が増えると小さく、減ると大きくなる

            worldTransforms_[i]->scale_ = Vector3(1.0f, xScaleFactor, 1.0f);
        }
        else {
            // 最初のポイントはデフォルトスケール
            worldTransforms_[i]->scale_ = Vector3(1.0f, 1.0f, 1.0f);
        }

        // ワールド行列を更新
        worldTransforms_[i]->UpdateMatrix();
    }
}


/// <summary>
/// 描画
/// </summary>
void Spline::Draw() {
    for (size_t i = 0; i < pointsDrawing_.size(); ++i) {
        if (i < obj_.size() && i < worldTransforms_.size()) {
            obj_[i]->Draw(*worldTransforms_[i]);
        }
    }
}

/// <summary>
/// Catmull-Rom スプラインでスムーズなポイントを生成
/// </summary>
std::vector<Vector3> Spline::GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t index, size_t segmentCount)
{
    std::vector<Vector3> splinePoints;

    const Vector3& p0 = controlPoints[(std::max)(static_cast<int>(index) - 1, 0)];
    const Vector3& p1 = controlPoints[index];
    const Vector3& p2 = controlPoints[index + 1];
    const Vector3& p3 = controlPoints[(std::min)(index + 2, controlPoints.size() - 1)];

    for (size_t i = 0; i <= segmentCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(segmentCount);
        Vector3 point = CatmullRomSpline({ p0, p1, p2, p3 }, t);
        splinePoints.push_back(point);
    }

    return splinePoints;
}

/// <summary>
/// 等間隔でサンプルされたスプラインポイントの生成
/// </summary>
std::vector<Vector3> Spline::SampleSplineEquidistant(const std::vector<Vector3>& points, size_t sampleCount)
{
    std::vector<float> distances(points.size(), 0.0f);
    for (size_t i = 1; i < points.size(); ++i) {
        distances[i] = distances[i - 1] + Magnitude(points[i] - points[i - 1]);
    }

    float totalDistance = distances.back();
    float interval = totalDistance / static_cast<float>(sampleCount - 1);

    std::vector<Vector3> sampledPoints;
    sampledPoints.reserve(sampleCount);
    sampledPoints.push_back(points.front());

    float currentDistance = interval;
    size_t currentIndex = 1;

    while (sampledPoints.size() < sampleCount - 1 && currentIndex < points.size()) {
        while (currentIndex < points.size() && currentDistance > distances[currentIndex]) {
            ++currentIndex;
        }

        if (currentIndex >= points.size()) {
            break;
        }

        float t = (currentDistance - distances[currentIndex - 1]) / (distances[currentIndex] - distances[currentIndex - 1]);
        Vector3 point = points[currentIndex - 1] * (1.0f - t) + points[currentIndex] * t;
        sampledPoints.push_back(point);
        currentDistance += interval;
    }

    sampledPoints.push_back(points.back());
    return sampledPoints;
}

