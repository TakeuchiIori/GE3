#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include "WorldTransform.h"

#include <memory>
#include <list>
// 行動フェーズ
enum class Phase {
	Approch, // 接近する
	Leave,   // 離脱する
};

class Player;
class Enemy
{
public:

	/// <summary>
	/// コンストクラタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();
private: // メンバ関数

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitApproch();

	/// <summary>
	/// 接近処理
	/// </summary>
	void UpdateApproch();

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitLeave();

	/// <summary>
	/// 離脱処理
	/// </summary>
	void UpdateLeave();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 敵を復活させる
	/// </summary>
	void Revive();

public: // アクセッサ

	/// <summary>
	/// プレイヤー
	/// </summary>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPosition();

	float Setradius() { return rad; }
	float GetRadius() { return rad; }
	bool GetIsAlive() const { return isAlive_; }

private:

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// オブジェクト3d
	Object3d* object3d_ = nullptr;

	
	Vector3 velocity_ = { 0,0,0 };

	// フェーズ
	Phase phase_ = Phase::Approch;

	// 自キャラ
	Player* player_ = nullptr;
	// 接近
	Vector3 kApprochSpeed = { 0.0f, 0.0f, -1.05f };
	// 離脱
	Vector3 kLeaveSpeed = { 0.0f, 0.0f, -0.01f }; //{-0.1f, 0.1f, 0.0f};

	// 半径
	static inline const float rad = 1.0f;

	bool isAlive_ = true;


private: 
	// 発射タイマー
	int32_t shotTImer_ = 0;
	// 発射感覚
	int32_t kFireInterval = 60;

	static void (Enemy::* phaseEnemy[])();

	int32_t respawnTimer_ = 0; // 復活待機タイマー
	static const int32_t kRespawnTime = 180; // 復活までの待機時間 (フレーム数) 
};

