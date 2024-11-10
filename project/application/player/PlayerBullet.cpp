#include "PlayerBullet.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "Camera.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <iostream>



PlayerBullet::PlayerBullet()
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Initialize(const Vector3& position,Vector3& velocity)
{
	ModelManager::GetInstance()->LoadModel("player.obj");
	object3d_ = new Object3d();
	object3d_->Initialize();
	object3d_->SetModel("player.obj");


	worldTransform_.Initialize();
	// 引数で受け取ったワールドトランスフォームをメンバ変数に代入
	worldTransform_.translation_ = position;
	// 引数で受け取っ速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::Update()
{
	if (--deathTimer_ <= 0) {
		isAlive_ = false;
	}
	// 座標を移動させる
	worldTransform_.translation_ += velocity_;
	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

}

void PlayerBullet::Draw()
{
	// モデル描画
	object3d_->Draw(worldTransform_);
}

void PlayerBullet::OnCollision()
{
	isAlive_ = false;
}

Vector3 PlayerBullet::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
