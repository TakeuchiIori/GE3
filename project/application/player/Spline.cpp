#include "Spline.h"
#include "ModelManager.h"

#include <vector>
#include <memory>
#include "Vector3.h"
#include "Object3d.h"
#include "WorldTransform.h"

void Spline::Initialize()
{
    // .objモデルの読み込み
    ModelManager::GetInstance()->LoadModel("rail.obj");

    // 制御点の初期化
    controlPoints_ = {
        {0, 0, 0},       // スタート地点
        {100, 150, 100},    // 上昇 (上方向に登る)
        {200, 500, 200},     // 下り (低くなる)
        {300, 200, 100},    // 再度上昇
        {400, -100, 0},    // 急下降
        {500, 50, -100},    // 緩やかな上昇
        {600, 300, -200},   // 高く登る
        {700, 0, 0},      // 下りながら進む
        {800, -150, 150},   // 再度急降下
        {900, 100, 250},    // ゆっくり上昇
        {1000, 0, 100},    // 平坦に進む
        {1100, -200, 50},   // さらに下降して終わる
    };

    // 各セグメント間の分割数を設定
    segmentCounts_ = { 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 }; // 任意の分割数

    // スプラインのポイントリストを生成
    std::vector<Vector3> allPoints;
    for (size_t i = 0; i < controlPoints_.size() - 1; ++i)
    {
        std::vector<Vector3> segmentPoints = GenerateCatmullRomSplinePoints(controlPoints_, i, segmentCounts_[i]);
        allPoints.insert(allPoints.end(), segmentPoints.begin(), segmentPoints.end());
    }

    // スプラインポイントを等間隔にサンプリング
   
    pointsDrawing_ = SampleSplineEquidistant(allPoints, sampleCount);

    // 各スプラインポイントに対してObject3dとWorldTransformを生成して初期化
    obj_.clear();
    worldTransforms_.clear();
    for (size_t i = 0; i < pointsDrawing_.size(); ++i)
    {
        // Object3dの生成と初期化
        auto obj = std::make_unique<Object3d>();
        obj->Initialize();
        obj->SetModel("rail.obj");
        obj_.push_back(std::move(obj));

        // WorldTransformの生成と初期化
        auto worldTransform = std::make_unique<WorldTransform>();
        worldTransform->Initialize();
        worldTransform->translation_ = pointsDrawing_[i]; // 各ポイントに基づいた初期位置を設定

        // 回転をクォータニオンで計算
        if (i > 0) {
            Vector3 direction = pointsDrawing_[i] - pointsDrawing_[i - 1];
            Vector3 nomdirection = Normalize(direction);
            Vector3 up(0.0f, 1.0f, 0.0f); // 基準の上方向
            Vector3 rotationAxis = Cross(up, nomdirection);
            float rotationAngle = std::acos(Dot(up, nomdirection));
            Quaternion rotation = MakeRotateAxisAngleQuaternion(rotationAxis, rotationAngle);
            Vector3 eulerRotation = QuaternionToEuler(rotation);
            worldTransform->rotation_ = eulerRotation; // オイラー角に基づいた回転を設定
        }
        worldTransforms_.push_back(std::move(worldTransform));
    }
}

void Spline::Update()
{
    // 各制御点に基づいてスプラインの再計算を行う
    std::vector<Vector3> allPoints;
    for (size_t i = 0; i < controlPoints_.size() - 1; ++i)
    {
        std::vector<Vector3> segmentPoints = GenerateCatmullRomSplinePoints(controlPoints_, i, segmentCounts_[i]);
        allPoints.insert(allPoints.end(), segmentPoints.begin(), segmentPoints.end());
    }

    // スプラインポイントを等間隔にサンプリング
   
    pointsDrawing_ = SampleSplineEquidistant(allPoints, sampleCount);

    // 既存のオブジェクトリストとワールドトランスフォームリストのサイズと一致しない場合はリサイズしない
    if (pointsDrawing_.size() != obj_.size() || pointsDrawing_.size() != worldTransforms_.size())
    {
        // サイズが一致しない場合は更新をスキップして描画もしない
        return;
    }

    // 各ワールドトランスフォームの行列を更新
    for (size_t i = 0; i < pointsDrawing_.size(); ++i)
    {
        worldTransforms_[i]->translation_ = pointsDrawing_[i];

        // 回転をクォータニオンで計算し直す
        if (i > 0) {
            Vector3 direction = pointsDrawing_[i] - pointsDrawing_[i - 1];
            Vector3 nomdirection = Normalize(direction);
            Vector3 up(0.0f, 1.0f, 0.0f); // 基準の上方向
            Vector3 rotationAxis = Cross(up, nomdirection);
            float rotationAngle = std::acos(Dot(up, nomdirection));
            Quaternion rotation = MakeRotateAxisAngleQuaternion(rotationAxis, rotationAngle);
            Vector3 eulerRotation = QuaternionToEuler(rotation);
            worldTransforms_[i]->rotation_ = eulerRotation; // オイラー角に基づいた回転を設定
        }

        // 行列の更新
        worldTransforms_[i]->UpdateMatrix();
    }
}

void Spline::Draw() {
    // 描画処理の実装を行う
    // 例: 全てのスプラインポイントに対して描画を行う
    for (size_t i = 0; i < pointsDrawing_.size(); ++i) {
        if (i < obj_.size() && i < worldTransforms_.size()) {
            obj_[i]->Draw(*worldTransforms_[i]);
        }
    }
}


std::vector<Vector3> Spline::GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t index, size_t segmentCount)
{
    std::vector<Vector3> splinePoints;

    // セグメントのインデックスから4つの制御点を取得（Catmull-Romでは4つの制御点が必要）
    const Vector3& p0 = controlPoints[(std::max)(static_cast<int>(index) - 1, 0)];
    const Vector3& p1 = controlPoints[index];
    const Vector3& p2 = controlPoints[index + 1];
    const Vector3& p3 = controlPoints[(std::min)(index + 2, controlPoints.size() - 1)];

    // 各セグメントを分割してポイントを生成
    for (size_t i = 0; i <= segmentCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(segmentCount);
        Vector3 point = CatmullRomSpline({ p0, p1, p2, p3 }, t);
        splinePoints.push_back(point);
    }

    return splinePoints;
}

std::vector<Vector3> Spline::SampleSplineEquidistant(const std::vector<Vector3>& points, size_t sampleCount)
{
    // 各セグメント間の累積距離を計算
    std::vector<float> distances(points.size(), 0.0f);
    for (size_t i = 1; i < points.size(); ++i) {
        distances[i] = distances[i - 1] + Magnitude(points[i] - points[i - 1]);
    }

    // 等間隔サンプリングのための目標間隔
    float totalDistance = distances.back();
    float interval = totalDistance / static_cast<float>(sampleCount - 1);

    // 等間隔のサンプル点を収集
    std::vector<Vector3> sampledPoints;
    sampledPoints.reserve(sampleCount); // sampleCount のサイズを予約することでパフォーマンスを向上

    sampledPoints.push_back(points.front()); // 最初の点を追加

    float currentDistance = interval;
    size_t currentIndex = 1;

    // 必要なサンプル数分だけ取得
    while (sampledPoints.size() < sampleCount - 1 && currentIndex < points.size()) {
        // 現在の距離が累積距離を超えるまで次のセグメントに移動
        while (currentIndex < points.size() && currentDistance > distances[currentIndex]) {
            ++currentIndex;
        }

        if (currentIndex >= points.size()) {
            break;
        }

        // 線形補間でサンプル位置を計算
        float t = (currentDistance - distances[currentIndex - 1]) / (distances[currentIndex] - distances[currentIndex - 1]);
        Vector3 point = points[currentIndex - 1] * (1.0f - t) + points[currentIndex] * t;
        sampledPoints.push_back(point);
        currentDistance += interval;
    }

    // 最後の点を追加
    sampledPoints.push_back(points.back());

    return sampledPoints;
}
