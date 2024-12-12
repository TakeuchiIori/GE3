#include "CollisionManager.h"
// C++
#include <assert.h>
#include <iostream>

// Engine
#include "GlobalVariables.h"
#include "Loaders./Model/ModelManager.h"
#include "CollisionTypeIdDef.h"

// C++
#include "MathFunc.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG




CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Initialize() {


	// モデル読み込み
	//ModelManager::GetInstance()->LoadModel("ICO.obj");
	isDrawCollider_ = false;
	GlobalVariables* globalvariables = GlobalVariables::GetInstance();
	const char* groupName = "Collider";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalvariables->AddItem(groupName, "Collider", isDrawCollider_);
}
void CollisionManager::UpdateWorldTransform() {

	ApplyGlobalVariables();

	// 非表示なら抜ける
	if (!isDrawCollider_) {
		return;
	}
	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		// 更新
		collider->UpdateWorldTransform();
	}



}
void CollisionManager::Draw() {
	// 非表示なら抜ける
	if (!isDrawCollider_) {
		return;
	}
	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		// 描画
		collider->Draw(obj_);
	}
}
void CollisionManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();

}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// 両方のIDが同じかどちらかがNoneだった場合早期リターン
	if (colliderA->GetTypeID() == colliderB->GetTypeID() ||
		colliderA->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kNone) ||
		colliderB->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kNone)) {
		return;
	}
	// コライダーAの座標を取得
	Vector3 PosA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Vector3 PosB = colliderB->GetCenterPosition();
	// 座標の差分ベクトル
	Vector3 subtract = PosB - PosA;
	// 座標AとBの距離を求める
	float distance = Length(subtract);
	float radiusSum = colliderA->Getradius() + colliderB->Getradius();

	// 球と球の交差判定
	std::cout << "Distance: " << distance << ", Radius Sum: " << radiusSum << std::endl;
	if (distance <= (colliderA->Getradius() + colliderB->Getradius())) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレーターBはイテレーターAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider) {
	colliders_.push_back(collider);
	std::cout << "Collider added: " << collider->GetTypeID() << std::endl;
}

void CollisionManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Collider";
	isDrawCollider_ = globalVariables->GetBoolValue(groupName, "Collider");

}
