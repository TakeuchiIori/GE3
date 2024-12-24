#pragma once
// Engine
#include "Object3D./Object3d.h"
#include "Systems/Input./Input.h"
#include "WorldTransform./WorldTransform.h"
#include "Collision./Collider.h"

// C++
#include <memory>

// Math
#include "MathFunc.h"
#include "Vector3.h" 

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
	/// キーボードで移動処理
	/// </summary>
	void MoveKey();

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

private: // メンバ変数

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> base_ = nullptr;
	Input* input_ = nullptr;

	bool isColliding_ = false;
	Vector3 moveSpeed_;
	bool isDrawEnabled_ = true;

};

