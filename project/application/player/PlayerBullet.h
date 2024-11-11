#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include <memory>
#include "WorldTransform.h"

class PlayerBullet
{

public:
	// コンストラクタ
	// デストラクタ
	PlayerBullet();
	~PlayerBullet();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position,Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デスタイマー関数
	/// </summary>
	bool IsAlive() const { return isAlive_; }

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	float GetRadius() { return rad_; }
	float Setradius() { return rad_; }

private:

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// オブジェクト
	Object3d* object3d_ = nullptr;

	// 速度
	Vector3 velocity_ = { 0,0,0 };
	// 寿命<frm>
	static const int32_t kLifeTime_ = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime_;
	// デスフラグ
	bool isAlive_ = true;
	// 半径
	static inline const float rad_ = 1.0f;
};

