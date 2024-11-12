#include "Player.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "Camera.h"
#include "Matrix4x4.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif

// C++
#include <iostream>
#include <string>


Player::Player()
{
}

Player::~Player()
{
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite_;
}

void Player::Initailize()
{
	// .obj読み込み
	ModelManager::GetInstance()->LoadModel("player.obj");

	// OBject3dの初期化
	base_ = std::make_unique<Object3d>();
	base_->Initialize();
	base_->SetModel("player.obj");

	// 2Dスプライト
	std::string textureFilePath = "Resources/2DReticle.png";
	sprite_ = new Sprite();
	sprite_->Initialize(textureFilePath);
	sprite_->AdjustTaxtureSize();
	sprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));

	// その他初期化
	input_ = Input::GetInstance();
	moveSpeed_ = { 0.5f, 0.5f , 0.5f };
	worldTransform_.Initialize();
	worldTransform3DReticle_.Initialize();
	worldTransform_.translation_.z = -70.0f;
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

	ReticleUpdate();

	ShowCoordinatesImGui();

	LastUpdate();
}

void Player::ReticleUpdate()
{
	POINT mousePosition;
	// マウスの座標を取得する
	GetCursorPos(&mousePosition);
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// スプライトのレティクル座標に設定
	sprite_->SetPosition(Vector2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)));

	// デフォルトカメラからビュープロジェクション行列を取得
	Camera* camera = Object3dCommon::GetInstance()->GetDefaultCamera();
	if (!camera) return; // カメラが存在しない場合は終了

	Matrix4x4 viewProjectionMatrix = camera->GetViewProjectionMatrix();

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matVPV = Multiply(viewProjectionMatrix, matViewport);

	// 合成行列の逆行列を計算
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// 2点のワールド行列
	posNear = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
	posFar = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);

	// スクリーン座標からワールド座標
	posNear = TransformCoordinates(posNear, matInverseVPV);
	posFar = TransformCoordinates(posFar, matInverseVPV);

	//----- 3Dレティクルの座標計算 -----//
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;

	worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;
	worldTransform3DReticle_.UpdateMatrix();

	sprite_->Update();
}

void Player::Draw()
{
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}
	base_->Draw(worldTransform_);
	// 使えない
	//base_->DrawLine(Vector3(0, 0, 0), Vector3(10, 10, 10), Vector4(1, 0, 0, 1));
	ReticleDraw();
}

void Player::ReticleDraw()
{
	sprite_->Draw();
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
	if (input_->IsPressMouse(4)) {
	    worldTransform_.translation_.y += moveSpeed_.z;
	}

	// マウスの押下をチェック
	if (input_->IsPressMouse(3)) {
	    worldTransform_.translation_.y -= moveSpeed_.z;
	}


	// プレイヤーの回転
	if (input_->PushKey(DIK_UP)) {
		worldTransform_.rotation_.x -= 0.05f; // 左方向に回転
	}
	if (input_->PushKey(DIK_DOWN)) {
		worldTransform_.rotation_.x += 0.05f; // 右方向に回転
	}

	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_.rotation_.y -= 0.05f; // 左方向に回転
	}
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_.rotation_.y += 0.05f; // 右方向に回転
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
Vector3 Player::GetReticleWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}
void Player::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Player::Fire()
{
	if (input_->PushKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;

		// プレイヤー位置とレティクル位置を取得
		Vector3 playerPosition = GetWorldPosition();
		Vector3 reticlePosition = GetReticleWorldPosition();

		// 発射方向を算出（プレイヤー位置からレティクル位置へ向かうベクトル）
		Vector3 direction = reticlePosition - playerPosition;
		direction = Normalize(direction) * kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(playerPosition, direction);

		// 弾をリストに追加
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
