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
    {0, 0, 0},           // スタート地点
    {50, 25, 25},        // 緩やかな上昇
    {100, 75, 50},       // 徐々に上昇
    {150, 100, 50},      // 少し下りながら進む
    {200, 25, 25},       // ゆっくり下降
    {250, 0, 0},         // 緩やかに降下
    {300, 50, -25},      // 少し上昇
    {350, 75, -50},      // 緩やかな下降
    {400, 50, 0},        // 滑らかに進む
    {450, 25, 50},       // 徐々に上昇
    {500, 0, 25},        // 緩やかに進む
    {550, -50, 0},       // 少し下降して終わる
    };

    // 各セグメント間の分割数を設定
    segmentCounts_ = { 30, 20, 20, 20, 20, 20, 20, 20, 20, 20, 30 };

    // 描画用スプラインポイントの生成
    for (size_t i = 0; i < controlPoints_.size() - 3; ++i) {
        auto segmentPoints = GenerateCatmullRomSplinePoints(controlPoints_, i, segmentCounts_[i]);
        pointsDrawing_.insert(pointsDrawing_.end(), segmentPoints.begin(), segmentPoints.end());
    }

    // 等間隔サンプリング
    pointsDrawing_ = SampleSplineEquidistant(pointsDrawing_, sampleCount);

    // 各スプラインポイントに対応するオブジェクトとワールド変換を生成
    for (size_t i = 0; i < pointsDrawing_.size(); ++i)
    {
        auto obj = std::make_unique<Object3d>();
        obj->Initialize();
        obj->SetModel("rail.obj");  // レールモデルのロード
        obj_.push_back(std::move(obj));

        auto worldTransform = std::make_unique<WorldTransform>();
        worldTransform->Initialize();
        worldTransform->translation_ = pointsDrawing_[i];

        if (i > 0) {
            Vector3 currentDirection = Normalize(pointsDrawing_[i] - pointsDrawing_[i - 1]);
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(1.0f, 0.0f, 0.0f), currentDirection);
            Vector3 eulerRotation = QuaternionToEuler(rotationQuat);
            eulerRotation.y += 1.56f; // カメラと同じ調整を加える
            worldTransform->rotation_ = eulerRotation;
        }
        else if (pointsDrawing_.size() > 1) {
            // 最初のポイントも次のポイントに基づいて回転を適用
            Vector3 currentDirection = Normalize(pointsDrawing_[1] - pointsDrawing_[0]);
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(1.0f, 0.0f, 0.0f), currentDirection);
            Vector3 eulerRotation = QuaternionToEuler(rotationQuat);
            eulerRotation.y += 1.56f; // カメラと同じ調整を加える
            worldTransform->rotation_ = eulerRotation;
        }

        // スケールの設定（始点から終点の距離を基に計算）
        if (i > 0) {
            float scale = Length(pointsDrawing_[i] - pointsDrawing_[i - 1]);
            worldTransform->scale_ = Vector3(scale, 1.0f, 1.0f);
        }
        else {
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
        // 各ポイントの位置を設定
        worldTransforms_[i]->translation_ = pointsDrawing_[i];

        if (i > 0) {
            // 前のポイントと現在のポイントを使用して方向と回転を計算
            Vector3 startPoint = pointsDrawing_[i - 1];
            Vector3 endPoint = pointsDrawing_[i];
            Vector3 direction = Normalize(endPoint - startPoint);

            // クォータニオンで回転を計算
            Quaternion rotationQuat = SetFromToQuaternion(Vector3(1.0f, 0.0f, 0.0f), direction);
            Vector3 eulerRotation = QuaternionToEuler(rotationQuat);
            eulerRotation.y += 1.56f; // 回転の微調整
            worldTransforms_[i]->rotation_ = eulerRotation;

            // スケールの設定
            float scale = Length(endPoint - startPoint);
            worldTransforms_[i]->scale_ = Vector3(scale , 1.0f, 1.0f);
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

