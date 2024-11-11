#include "EnemyBullet.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "Camera.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <iostream>

EnemyBullet::EnemyBullet()
{
}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Initialize(const Vector3& position, Vector3& velocity)
{
	ModelManager::GetInstance()->LoadModel("player.obj");

	// オブジェクト初期化
	object3d_ = new Object3d();
	object3d_->Initialize();
	object3d_->SetModel("Player.obj");

	// ワールドトランスフォーム初期化	
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;



}

void EnemyBullet::Update()
{
	if (--deathTimer_ <= 0) {
		isAlive_ = false;
	}

	// 座標を移動させる
	worldTransform_.translation_ += velocity_;
	// ワールドトランスフォームの更新
	worldTransform_.Update();
}

void EnemyBullet::Draw()
{
	object3d_->Draw(worldTransform_);
}

void EnemyBullet::OnCollision()
{
	isAlive_ = false;
}

Vector3 EnemyBullet::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
