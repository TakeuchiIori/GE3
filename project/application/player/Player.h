#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include "WorldTransform.h"
#include "PlayerBullet.h"
#include "Sprite.h"
// C++
#include <memory>
#include <list>
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
	/// レティクルの更新
	/// </summary>
	void ReticleUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// レティクルの描画
	/// </summary>
	void ReticleDraw();
		

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
	
	Vector3 GetWorldPosition();
	Vector3 GetReticleWorldPosition();
	const Vector3& GetPosition() const { return worldTransform_.translation_; }
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	
	float Setradius() { return rad_; }
	float GetRadius() { return rad_; }

	
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetParent(const WorldTransform* parent);
private: 

	// 自機のワールドトランスフォーム
	WorldTransform worldTransform_;

	// レティクルのワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;
	// 2Dスプライト
	Sprite* sprite_;
	Vector3 posNear;
	Vector3 posFar;

	std::unique_ptr<Object3d> base_ = nullptr;
	Input* input_ = nullptr;

	Vector3 moveSpeed_;

	// 半径
	static inline const float rad_ = 1.0f;

	// 弾
	std::list<PlayerBullet*> bullets_;
};

