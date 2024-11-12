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
        {0, 0, 0},        // スタート地点
        {100, 150, 100},  // 上昇
        {200, 500, 200},  // 下り
        {300, 200, 100},  // 再度上昇
        {400, -100, 0},   // 急下降
        {500, 50, -100},  // 緩やかな上昇
        {600, 300, -200}, // 高く登る
        {700, 0, 0},      // 下りながら進む
        {800, -150, 150}, // 急降下
        {900, 100, 250},  // ゆっくり上昇
        {1000, 0, 100},   // 平坦に進む
        {1100, -200, 50}, // さらに下降して終わる
    };

    // 各セグメント間の分割数を設定
    segmentCounts_ = { 300, 200, 200, 200, 200, 200, 200, 200, 200, 200, 300 };

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

        // 回転の計算
        if (i > 0) {
            // X軸方向 (1, 0, 0) から currentDirection への回転を求める
            Vector3 currentDirection = Normalize(pointsDrawing_[i] - pointsDrawing_[i - 1]);
            Vector3 eulerRotation = SetFromTo(Vector3(1.0f, 0.0f, 0.0f), currentDirection);
            worldTransform->rotation_ = eulerRotation;
        }
        else {
            // 最初のポイントは回転なし
            worldTransform->rotation_ = Vector3(0.0f, 0.0f, 0.0f);
        }

        worldTransforms_.push_back(std::move(worldTransform));
    }
}

/// <summary>
/// 更新処理
/// </summary>
/// <summary>
/// 更新処理
/// </summary>
void Spline::Update()
{
    // 初期の回転を保持するクォータニオン
    Quaternion lastRotation = Quaternion::Identity();

    for (size_t i = 0; i < pointsDrawing_.size(); ++i)
    {
        // 各ポイントの位置を設定
        worldTransforms_[i]->translation_ = pointsDrawing_[i];

        if (i > 0) {
            // 現在のポイントと前のポイントの方向ベクトルを計算
            Vector3 currentDirection = Normalize(pointsDrawing_[i] - pointsDrawing_[i - 1]);

            // 前回の回転から現在の方向に向かうクォータニオンを計算
            Quaternion targetRotation = SetFromToQuaternion(Vector3(1.0f, 0.0f, 0.0f), currentDirection);

            // 連続性を保つために、lastRotation と targetRotation の符号を揃える
            if (Dot(lastRotation, targetRotation) < 0.0f) {
                targetRotation = Quaternion(-targetRotation.x, -targetRotation.y, -targetRotation.z, -targetRotation.w);
            }

            // 前の回転とターゲット回転を補間してスムーズな回転を生成
            float t = static_cast<float>(i) / pointsDrawing_.size(); // 線形に補間
            Quaternion interpolatedRotation = Slerp(lastRotation, targetRotation, t);

            // クォータニオンをオイラー角に変換して回転を設定
            Vector3 eulerRotation = QuaternionToEuler(interpolatedRotation);
            worldTransforms_[i]->rotation_ = eulerRotation;

            // 次のポイントのために現在の回転を保持
            lastRotation = interpolatedRotation;
        }
        else {
            // 最初のポイントは回転なし
            worldTransforms_[i]->rotation_ = Vector3(0.0f, 0.0f, 0.0f);
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
