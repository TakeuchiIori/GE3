#pragma once
// Engine
#include "WorldTransform./WorldTransform.h"
#include "Object3D./Object3d.h"
#include "Collision./CollisionTypeIdDef.h"

// Math
#include "Vector3.h"

class Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ワールドトランスフォームの初期化
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Object3d* obj);

public: // ポリモーフィズム

	virtual ~Collider() = default;

	// 衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]] Collider* other) {}

	// 中心座標を取得
	virtual Vector3 GetCenterPosition() const = 0;





public: // アクセッサ

	/// <summary>
	/// ゲッター
	/// </summary>
	float Getradius() { return radius_; }

	/// <summary>
	/// セッター
	/// </summary>
	void Setradius(const float& radius) { radius_ = radius; }

	/// <summary>
	///  種別IDを取得
	/// </summary>
	uint32_t GetTypeID() const { return typeID_; }

	/// <summary>
	///  種別IDを\設定
	/// </summary>
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }


private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// 衝突判定
	float radius_ = 1.5f;
	// 種別ID
	uint32_t typeID_ = 0u;


};
