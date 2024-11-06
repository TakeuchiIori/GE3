#pragma once
#include "MathFunc.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include <vector>
#include <memory>
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
	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> obj_;


	std::vector<Vector3> controlPoints_;
	std::vector<Vector3> pointsDrawing_;
	const size_t segmentCount = 100;

};

