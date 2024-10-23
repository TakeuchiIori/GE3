#include "Player.h"
#include "ModelManager.h"
#include "Object3dCommon.h"

void Player::Initailize()
{
	// .obj読み込み
	ModelManager::GetInstance()->LoadModel("player.obj");

	// OBject3dの初期化
	base_ = make_unique<Object3d>();
	base_->Initialize(Object3dCommon::Getinstance());
	base_->SetModel("player.obj");

	// その他初期化
	input_ = Input::GetInstance();
	moveSpeed_ = { 0.5f,0.5f ,0.5f };

	transform_ = base_->GetTransform();
}

void Player::Update()
{
	Move();
	

	LastUpdate();
}

void Player::Draw()
{
	base_->Draw();
}

void Player::LastUpdate()
{

	// 最後に必ずセット
	base_->SetScale(transform_.scale);
	base_->SetRotation(transform_.rotate);
	base_->SetPosition(transform_.translate);
	base_->Update();
}

void Player::Move()
{
	// キーボード
	if (input_->PushKey(DIK_W)) {
		transform_.translate.z += moveSpeed_.z;
	}
	if (input_->PushKey(DIK_A)) {
		transform_.translate.x -= moveSpeed_.y;
	}
	if (input_->PushKey(DIK_S)) {
		transform_.translate.z -= moveSpeed_.z;
	}
	if (input_->PushKey(DIK_D)) {
		transform_.translate.x += moveSpeed_.y;
	}
}
