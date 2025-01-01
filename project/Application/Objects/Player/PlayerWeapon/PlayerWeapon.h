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

class PlayerWeapon
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///  更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:





public:
	
	void SetParent(WorldTransform& worldTransform) { worldTransform_.parent_ = &worldTransform; }


	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const Vector3& GetScale() { return worldTransform_.scale_; }
	const Vector3& GetRotation() { return worldTransform_.rotation_; }
	const Vector3& GetTranslation() { return worldTransform_.translation_; }

private:

	std::unique_ptr<Object3d> weapon_;
	WorldTransform worldTransform_;

};

