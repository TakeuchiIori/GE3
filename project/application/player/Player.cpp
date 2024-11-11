#include "Player.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "Camera.h"
#include "Matrix4x4.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <iostream>



Player::Player()
{
}

Player::~Player()
{
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

}

void Player::Initailize()
{
	// .obj読み込み
	ModelManager::GetInstance()->LoadModel("player.obj");

	// OBject3dの初期化
	base_ = std::make_unique<Object3d>();
	base_->Initialize();
	base_->SetModel("player.obj");

	// その他初期化
	input_ = Input::GetInstance();
	moveSpeed_ = { 0.5f, 0.5f , 0.5f };
	worldTransform_.Initialize();
}

void Player::Update()
{
	BulletDelete();

	Move();

	Fire();

	// 弾の更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	ShowCoordinatesImGui();
	LastUpdate();
}

void Player::Draw()
{
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}
	base_->Draw(worldTransform_);
}

void Player::OnCollision()
{
}

void Player::LastUpdate()
{
	// 最後に必ずセット
	worldTransform_.UpdateMatrix();
}

void Player::Move()
{
	// キーボード
	if (input_->PushKey(DIK_W)) {
		worldTransform_.translation_.z += moveSpeed_.z;
	}
	if (input_->PushKey(DIK_A)) {
		worldTransform_.translation_.x -= moveSpeed_.y;
	}
	if (input_->PushKey(DIK_S)) {
		worldTransform_.translation_.z -= moveSpeed_.z;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.translation_.x += moveSpeed_.y;
	}

	// マウスの押下をチェック
	if (input_->IsPressMouse(0)) {
	    worldTransform_.translation_.y += moveSpeed_.z;
	}

	// マウスの押下をチェック
	if (input_->IsPressMouse(1)) {
	    worldTransform_.translation_.y -= moveSpeed_.z;
	}

	// コントローラーの入力による移動処理
	XINPUT_STATE state;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, state)) {
		return;
	}

	if (input_->GetJoystickState(0, state)) {
		// 左スティックのX軸とY軸の値を取得し、プレイヤーの移動に反映
		float LX = (float)state.Gamepad.sThumbLX;
		float LY = (float)state.Gamepad.sThumbLY;


		worldTransform_.translation_.x += (LX / SHRT_MAX) * moveSpeed_.x;
		worldTransform_.translation_.z += (LY / SHRT_MAX) * moveSpeed_.z;


		// Aボタンでジャンプ処理
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) {
			worldTransform_.translation_.y += moveSpeed_.y;
		}

		// Bボタンでしゃがみ処理
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			worldTransform_.translation_.y -= moveSpeed_.y;
		}
	}
}
void Player::BulletDelete()
{
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsAlive()) {
			return false;
		}
		delete bullet;
		return true;
	});

}
Vector3 Player::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
void Player::Fire()
{
	if (input_->PushKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		//velocity = GetReticleWorldPosition() - GetWorldPosition();
		velocity = Normalize(velocity) * kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(GetWorldPosition(), velocity);
		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}



void Player::ShowCoordinatesImGui()
{
#ifdef _DEBUG
	// ImGuiウィンドウを利用してプレイヤーの座標を表示
	ImGui::Begin("Player Coordinates");
	ImGui::Text("Position X: %.2f", worldTransform_.translation_.x);
	ImGui::Text("Position Y: %.2f", worldTransform_.translation_.y);
	ImGui::Text("Position Z: %.2f", worldTransform_.translation_.z);
	ImGui::End();
#endif
}
