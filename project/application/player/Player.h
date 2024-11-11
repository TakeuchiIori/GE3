#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include <memory>
#include <list>
#include "WorldTransform.h"
#include "PlayerBullet.h"
class Player
{

public: // メンバ関数（公開）

	// コンストラクタ
	// デストラクタ
	Player();
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initailize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGui
	/// </summary>
	void ShowCoordinatesImGui();

	/// <summary>
	/// 弾リスト取得
	/// </summary>
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();


private: // メンバ関数（非公開）

	/// <summary>
	/// 更新の末端でやる処理
	/// </summary>
	void LastUpdate();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 射撃
	/// </summary>
	void Fire();

	/// <summary>
	/// 弾を削除
	/// </summary>
	void BulletDelete();

	

public:
	// プレイヤーの位置を取得する関数
	Vector3 GetWorldPosition();
	const Vector3& GetPosition() const { return worldTransform_.translation_; }
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	
	float Setradius() { return rad; }
	float GetRadius() { return rad; }

	// プレイヤーの回転を設定する関数
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	
private: 

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> base_ = nullptr;
	Input* input_ = nullptr;

	Vector3 moveSpeed_;

	// 半径
	static inline const float rad = 1.0f;

	// 弾
	std::list<PlayerBullet*> bullets_;
};

