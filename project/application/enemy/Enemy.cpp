#include "Enemy.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "Camera.h"
#include "Matrix4x4.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <iostream>

Enemy::Enemy() {

}

Enemy::~Enemy() {
}

void(Enemy::* Enemy::phaseEnemy[])() = {
	&Enemy::UpdateApproch,
	&Enemy::UpdateLeave

};


void Enemy::Initialize(const Vector3& position)
{
	// .obj読み込み
	ModelManager::GetInstance()->LoadModel("enemy.obj");

	// オブジェクト初期化
	object3d_ = new Object3d();
	object3d_->Initialize();
	object3d_->SetModel("enemy.obj");

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 接近フェーズ
	//InitApproch();
}

void Enemy::Update()
{
	//// メンバ関数ポインタの呼び出し
	//(this->*phaseEnemy[static_cast<size_t>(phase_)])();
	//Matrix4x4 moveMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	//// ワールド行列に代入
	//worldTransform_.matWorld_ = moveMatrix;

	if (!isAlive_) {
		Revive();
	}
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

	ImGui::Begin("Enemy");
	ImGui::Text("isAlive_: %s", isAlive_ ? "true" : "false");
	ImGui::End();

}

void Enemy::Draw()
{
	if (isAlive_) {
		object3d_->Draw(worldTransform_);
	}

}

void Enemy::OnCollision()
{
	isAlive_ = false;
}

void Enemy::InitApproch()
{
}

void Enemy::UpdateApproch()
{
}

void Enemy::InitLeave()
{
}

void Enemy::UpdateLeave()
{
}

void Enemy::Fire()
{

}

void Enemy::Revive()
{

	// 復活待機タイマーをカウントダウン
	respawnTimer_++;
	// 待機時間が経過したら復活
	if (respawnTimer_ >= kRespawnTime) {
		isAlive_ = true;
		respawnTimer_ = 0; // 復活タイマーをリセット
	}

}

Vector3 Enemy::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得　（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
