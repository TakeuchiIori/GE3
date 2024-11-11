#include "Spline.h"
#include "ModelManager.h"

void Spline::Initialize()
{
	// .objモデルの読み込み
	ModelManager::GetInstance()->LoadModel("player.obj");

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

		{ 0, 0, 0 },
		{ 0, 0, 0 }
	};

	// スプラインのポイントリストを生成
	std::vector<Vector3> allPoints = GenerateCatmullRomSplinePoints(controlPoints_, segmentCount);

	// スプラインポイントを等間隔にサンプリング
	
	pointsDrawing_ = SampleSplineEquidistant(allPoints, interval);

	// 各スプラインポイントに対してObject3dとWorldTransformを生成して初期化
	obj_.clear();
	worldTransforms_.clear();
	for (const auto& point : pointsDrawing_)
	{
		// Object3dの生成と初期化
		auto obj = std::make_unique<Object3d>();
		obj->Initialize();
		obj->SetModel("player.obj");
		obj_.push_back(std::move(obj));

		// WorldTransformの生成と初期化
		auto worldTransform = std::make_unique<WorldTransform>();
		worldTransform->Initialize();
		worldTransform->translation_ = point; // 各ポイントに基づいた初期位置を設定
		worldTransforms_.push_back(std::move(worldTransform));
	}
}

void Spline::Update()
{
	// スプラインポイントの再生成
	std::vector<Vector3> allPoints = GenerateCatmullRomSplinePoints(controlPoints_, segmentCount);

	pointsDrawing_ = SampleSplineEquidistant(allPoints, interval);

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
		worldTransforms_[i]->UpdateMatrix();
	}
}

void Spline::Draw()
{
	// 描画用頂点リストとオブジェクトリストとワールドトランスフォームリストのサイズが一致するかチェック
	if (pointsDrawing_.size() != obj_.size() || pointsDrawing_.size() != worldTransforms_.size()) {
		// サイズが一致しない場合は描画しない
		return;
	}

	// ワールド変換を適用して各点を描画
	for (size_t i = 0; i < pointsDrawing_.size(); ++i)
	{
		worldTransforms_[i]->translation_ = pointsDrawing_[i];
		obj_[i]->Draw(*worldTransforms_[i]);
	}
}

std::vector<Vector3> Spline::GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t segmentCount)
{
	std::vector<Vector3> splinePoints;

	if (controlPoints.size() < 4)
	{
		// スプラインの生成には少なくとも4つの制御点が必要です
		return splinePoints;
	}

	for (size_t i = 1; i < controlPoints.size() - 2; ++i)
	{
		for (size_t j = 0; j < segmentCount; ++j)
		{
			float t = static_cast<float>(j) / static_cast<float>(segmentCount);
			Vector3 p0 = controlPoints[i - 1];
			Vector3 p1 = controlPoints[i];
			Vector3 p2 = controlPoints[i + 1];
			Vector3 p3 = controlPoints[i + 2];

			// Catmull-Rom スプラインの式
			float t2 = t * t;
			float t3 = t2 * t;

			Vector3 point = 0.5f * ((2.0f * p1) +
				(-1 * p0 + p2) * t +
				(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
				(-1 * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);

			splinePoints.push_back(point);
		}
	}

	return splinePoints;
}
// 等間隔でポイントを抽出する関数
std::vector<Vector3> Spline::SamplePoints(const std::vector<Vector3>& points, size_t step)
{
	std::vector<Vector3> sampledPoints;

	// 指定した間隔でポイントを抽出
	for (size_t i = 0; i < points.size(); i += step)
	{
		sampledPoints.push_back(points[i]);
	}

	// 最後のポイントも追加する
	if (!points.empty() && (points.size() - 1) % step != 0)
	{
		sampledPoints.push_back(points.back());
	}

	return sampledPoints;
}

std::vector<Vector3> Spline::SampleSplineEquidistant(const std::vector<Vector3>& points, float interval)
{
	std::vector<Vector3> sampledPoints;

	if (points.size() < 2)
	{
		// 少なくとも2つのポイントが必要です
		return sampledPoints;
	}

	sampledPoints.push_back(points[0]); // 最初のポイントを追加
	float accumulatedDistance = 0.0f;

	for (size_t i = 1; i < points.size(); ++i)
	{
		float segmentDistance = Distance(points[i - 1], points[i]);
		accumulatedDistance += segmentDistance;

		// 間隔に達するまでループ
		while (accumulatedDistance >= interval)
		{
			// 前のポイントと現在のポイントの間を補間して等間隔の位置を計算
			float t = (accumulatedDistance - interval) / segmentDistance;
			Vector3 newPoint = {
				points[i - 1].x * (1 - t) + points[i].x * t,
				points[i - 1].y * (1 - t) + points[i].y * t,
				points[i - 1].z * (1 - t) + points[i].z * t
			};
			sampledPoints.push_back(newPoint);

			accumulatedDistance -= interval;
		}
	}

	return sampledPoints;
}