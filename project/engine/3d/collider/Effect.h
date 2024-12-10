#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "object3d/Object3d.h"
#include <memory>
class Effect {
public:
	Effect() = default;
	~Effect() = default;
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
	/// ワールドトランスフォーム
	/// </summary>
	void SetWorldTransform(const WorldTransform& worldTransform) { worldTransform_.parent_ = &worldTransform; }


	bool GetIsAlive() { return isAlive_; }

private:
	std::unique_ptr<Object3d> obj_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	bool isAlive_ = true;
	float timer_ = 15.0f;
};
