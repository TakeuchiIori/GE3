#pragma once
#include "Object3d.h"
#include "MathFunc.h"
#include "Vector3.h" 
#include "Input.h"
#include <memory>
#include "WorldTransform.h"

class Player
{

public: // メンバ関数（公開）

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

private: // メンバ関数（非公開）

	/// <summary>
	/// 更新の末端でやる処理
	/// </summary>
	void LastUpdate();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();


public: // アクセッサ
	// プレイヤーの位置を取得する関数
	const Vector3& GetPosition() const { return worldTransform_.translation_; }


private: // メンバ変数

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> base_ = nullptr;
	Input* input_ = nullptr;

	Vector3 moveSpeed_;

};

