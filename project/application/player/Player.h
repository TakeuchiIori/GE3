#pragma once
#include "object3d/Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include <memory>
#include "WorldTransform.h"
#include "collider/Collider.h"

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

