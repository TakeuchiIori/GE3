#include "Collider.h"
#include "collider/CollisionManager.h"

void Collider::Initialize() {
	worldTransform_.Initialize();
	CollisionManager::GetInstance()->AddCollider(this);
}

void Collider::UpdateWorldTransform() {
	// ワールド座標をワールドトランスフォームに適用
	worldTransform_.translation_ = GetCenterPosition();
	// ワールドトランスフォームを再計算して定数バッファに書き込む
	worldTransform_.UpdateMatrix();
}

void Collider::Draw(Object3d* obj) {
	obj->Draw(worldTransform_);
}


