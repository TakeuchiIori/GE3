#pragma once
// Engine
#include "Object3D./Object3d.h"
#include "Sprite/Sprite.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Input./Input.h"
#include "WorldTransform./WorldTransform.h"
#include "Collision./Collider.h"
#include "PlayerWeapon/PlayerWeapon.h"

#include <Particle/ParticleManager.h>
#include <Particle/ParticleEmitter.h>

// C++
#include <memory>

// Math
#include "MathFunc.h"
#include "Vector3.h" 

class PlayerWeapon;
class Player : public Collider
{

public: // メンバ関数（公開）

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



	/// <summary>
	/// ImGui
	/// </summary>
	void ShowCoordinatesImGui();

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

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetWorldMatrix() const override;

	/// <summary>
	/// 調整項目の保存
	/// </summary>
	void ApplyGlobalVariables();



private: // メンバ関数（非公開）

	/// <summary>
	/// 更新の末端でやる処理
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// 移動処理をまとめた関数
	/// </summary>
	void Move();

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	void MoveController();

	/// <summary>
	/// キーボードで移動処理
	/// </summary>
	void MoveKey();

	/// <summary>
	/// ジャンプ
	/// </summary>
	void Jump();

	/// <summary>
	/// ダッシュ
	/// </summary>
	void Dash();

public: // コマンドパターンによる移動関数

	/// <summary>
	/// 前に進む
	/// </summary>
	void MoveFront();

	/// <summary>
	/// 後ろに進む
	/// </summary>
	void MoveBehind();

	/// <summary>
	/// 右に進む
	/// </summary>
	void MoveRight();

	/// <summary>
	/// 左に進む
	/// </summary>
	void MoveLeft();



public: // アクセッサ
	// プレイヤーの位置を取得する関数
	const Vector3& GetPosition() const { return worldTransform_.translation_; }
	const Vector3& GetRotation() const { return worldTransform_.rotation_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	PlayerWeapon* GetPlayerWeapon() { return weapon_.get(); }

	void SetCameraSprite(Camera* camera) { sprite_->SetCamera(camera); }
	void SetCamera(Camera* camera) { camera_ = camera; }

private: 
	/*===============================================================//
							ワールドトランスフォーム
	//===============================================================*/

	WorldTransform worldTransform_;
	WorldTransform WS_;
	Camera* camera_;
	/*===============================================================//
								ポインタ
	//===============================================================*/
	std::unique_ptr<Object3d> base_ = nullptr;
	Input* input_ = nullptr;
	std::unique_ptr<PlayerWeapon> weapon_;
	std::unique_ptr<Object3d> shadow_;
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<ParticleEmitter> particleEmitter_;

	/*===============================================================//
								フラグ関連
	//===============================================================*/
	bool isColliding_ = false;
	Vector3 moveSpeed_;
	bool isDrawEnabled_ = true;
	bool isUpdate_ = true;

	/*===============================================================//
								ジャンプ関連
	//===============================================================*/
	bool isJumping_ = false;       // ジャンプ中かどうか
	float jumpVelocity_ = 0.0f;    // ジャンプの上昇速度
	const float gravity_ = -9.8f;  // 重力加速度
	const float groundY_ = 2.0f;   // 地面の高さ
	const float jumpPower_ = 5.0f; // ジャンプ力
	float jumpHeight_ = jumpPower_;
	float jumpTime_ = 0.0f;           // ジャンプ開始からの経過時間
	float jumpDuration_ = 1.0f; // ジャンプの補完時間
	float fallSpeedFactor_ = 1.5f;

	/*===============================================================//
								ダッシュ関連
	//===============================================================*/
	bool isDash_ = false;
	float dashTime_ = 0.0f;          // ダッシュの経過時間
	const float dashDuration_ = 0.4f; // ダッシュの継続時間
	const float dashSpeed_ = 100.0f;   // ダッシュの速度

	/*===============================================================//
								生存関連
	//===============================================================*/	
	uint32_t maxHP_ = 100;
	uint32_t hp_ = 100;
	bool isAlive_ = true;
};

