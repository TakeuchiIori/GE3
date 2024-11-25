#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include <memory>
#include "WorldTransform.h"
#include "collider/Collider.h"

class Enemy : public Collider
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

public: // ポリモーフィズム

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 中心座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetCenterPosition() const override;


private:

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> base_ = nullptr;


};

