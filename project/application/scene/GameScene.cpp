#include "GameScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "MathFunc.h"
#ifdef _DEBUG
#include "imgui.h"
#endif
#include <TextureManager.h>

void GameScene::Initialize()
{
	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("float_body.obj");

	// カメラの初期化
	currentCamera_ = cameraManager_.AddCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());

	// オブジェクト生成
	player_ = std::make_unique<Player>();
	player_->Initailize();

	for (int i = 0; i < 1; ++i) {
		Enemy* enemy = new Enemy();
		enemy->Initialize(Vector3{ 0,0,-12 });
		enemies_.push_back(enemy);
	}

	test_ = std::make_unique<Object3d>();
	test_->Initialize();
	test_->SetModel("float_body.obj");
	testWorldTransform_.Initialize();

	spline_ = std::make_unique<Spline>();
	spline_->Initialize();
	// 初期カメラモード設定
	cameraMode_ = CameraMode::FPS;
}

void GameScene::Finalize()
{
	FinalizeCamera();
}

void GameScene::Update()
{
	// シーン切り替え
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	CheackAllCollisions();

	// 各オブジェクトの更新
	player_->Update();
	spline_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	// カメラの更新
	UpdateCameraMode();
	UpdateCamera();
	UpdateCameraWithRightStick();

	// ワールドトランスフォーム更新
	testWorldTransform_.UpdateMatrix();
	cameraManager_.UpdateAllCameras();
}

void GameScene::Draw()
{
	PrepareDraw();

	// 各オブジェクトの描画
	player_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
	test_->Draw(testWorldTransform_);
	spline_->Draw();
}

// 仮のやつ
void GameScene::CheackAllCollisions()
{
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition(); // 敵弾の座標を取得

		float distance = Length(posA - posB);

		// 球と球の交差判定
		if (distance <= (player_->GetRadius() + bullet->GetRadius())) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	// 自弾と敵キャラの当たり判定
	for (Enemy* enemy : enemies_) {
		posA = enemy->GetWorldPosition();

		for (PlayerBullet* bullet : playerBullets) {
			posB = bullet->GetWorldPosition();
			float distance = Length(posA - posB);

			if (distance <= enemy->GetRadius() + bullet->GetRadius()) {
				if (enemy->GetIsAlive()) {
					enemy->OnCollision();
					bullet->OnCollision();
				}
			}

		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets_) {
			posA = playerBullet->GetWorldPosition();
			posB = enemyBullet->GetWorldPosition();

			float distance = Length(posA - posB);

			if (distance <= playerBullet->GetRadius() + enemyBullet->GetRadius()) {
				// 敵弾のコールバックを呼び出す
				enemyBullet->OnCollision();
				// 自弾のコールバックを呼び出す
				playerBullet->OnCollision();
			}
		}
	}
#pragma endregion

}

void GameScene::FinalizeCamera()
{
	cameraManager_.RemoveCamera(currentCamera_);
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
}


void GameScene::UpdateCameraMode()
{
#ifdef _DEBUG
	ImGui::Begin("Camera Mode");
	if (ImGui::Button("Follow Camera")) {
		cameraMode_ = CameraMode::FOLLOW;
	}
	if (ImGui::Button("Top-Down Camera")) {
		cameraMode_ = CameraMode::TOP_DOWN;
	}
	if (ImGui::Button("FPS Camera")) {
		cameraMode_ = CameraMode::FPS;
	}
	if (ImGui::Button("DEBUG Camera")) {
		cameraMode_ = CameraMode::DEBUG;
	}
	ImGui::End();
#endif
}

void GameScene::UpdateCamera()
{
	switch (cameraMode_)
	{
	case CameraMode::FOLLOW:
	{
		Vector3 playerPos = player_->GetPosition();
		currentCamera_->FollowCamera(playerPos);
	}
	break;
	case CameraMode::TOP_DOWN:
	{
		Vector3 topDownPosition = Vector3(0.0f, 100.0f, 0.0f);
		currentCamera_->SetTopDownCamera(topDownPosition + player_->GetPosition());
	}
	break;
	case CameraMode::FPS:
		// プレイヤーの位置と回転を取得してFPS視点にセット
		// 調整中。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
		currentCamera_->SetFPSCamera(player_->GetPosition(), player_->GetRotation());

		break;

	case CameraMode::DEBUG:

		currentCamera_->DebugCamera(player_->GetPosition(),10.0f, 0.001f);

		break;
	default:
		break;
	}

}

/// <summary>
/// 右スティックの入力に基づきカメラを操作し、プレイヤーの向きも同期させる
/// </summary>
void GameScene::UpdateCameraWithRightStick()
{
	XINPUT_STATE state;
	if (Input::GetInstance()->GetJoystickState(0, state)) {
		float rightStickX = state.Gamepad.sThumbRX / 5000.0f;
		float rightStickY = state.Gamepad.sThumbRY / 5000.0f;

		// カメラを右スティックで回転
		if (rightStickX != 0.0f || rightStickY != 0.0f) {
			currentCamera_->ControlCameraWithRightStick(rightStickX, rightStickY, 0.1f);

			// カメラの向きに基づいてプレイヤーの向きを設定
			Vector3 cameraRotation = currentCamera_->GetRotate();
			player_->SetRotation(Vector3(cameraRotation.x, cameraRotation.y, 0.0f)); // 水平方向のみ同期
		}
	}
}


void GameScene::PrepareDraw()
{
	Object3dCommon::GetInstance()->DrawPreference();
	SpriteCommon::GetInstance()->DrawPreference();
}
