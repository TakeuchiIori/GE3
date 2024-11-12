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

	ModelManager::GetInstance()->LoadModel("skydome_sphere.obj");
	
	// カメラの初期化
	currentCamera_ = cameraManager_.AddCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());

	// オブジェクト生成
	player_ = std::make_unique<Player>();
	player_->Initailize();


	test_ = std::make_unique<Object3d>();
	test_->Initialize();
	test_->SetModel("skydome_sphere.obj");
	testWorldTransform_.Initialize();

	splineIndex_ = 0;  // スプラインインデックスの初期化
	spline_ = std::make_unique<Spline>();
	spline_->Initialize();

	// スプライン近くに敵をスポーン
	for (int i = 0; i < 10; ++i) { // 例えば5体の敵を生成
		SpawnEnemyNearSpline();
	}
	

	// カメラを更新
	cameraManager_.AddCamera();

	// 初期カメラモード設定
	cameraMode_ = CameraMode::FPS;

	//railCamera_ = make_unique<RailCamera>();
	//railCamera_->SetSplinePath(spline_->GetSplinePoints()); // スプラインパスを設定
	//railCamera_->SetSpeed(10.05f); // 移動速度の設定
	//// プレイヤーをレールカメラにペアレント
	//player_->SetParent(&railCamera_->GetWorldTransform());
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


	//railCamera_->Update();

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
	//railCamera_->Update();
	float speedFactor = 0.3f;
	// プレイヤーをスプラインに沿って移動させる
	if (splineIndex_ < spline_->GetSplinePoints().size()) {
		// 現在のスプライン位置と次のスプライン位置を取得
		Vector3 currentPoint = spline_->GetSplinePoints()[splineIndex_];
		Vector3 nextPoint;

		if (splineIndex_ < spline_->GetSplinePoints().size() - 1) {
			nextPoint = spline_->GetSplinePoints()[splineIndex_ + 1];
		}
		else {
			nextPoint = spline_->GetSplinePoints()[0]; // スプラインのループ対応
		}

		// プレイヤーの位置をスプラインの現在のポイントに配置
		player_->SetPosition(currentPoint);

		// 次のポイントに向かう方向を計算
		Vector3 direction = nextPoint - currentPoint;

		// 方向ベクトルを正規化
		direction = Normalize(direction);

		// 水平方向のY軸回転角度を計算する
		float yRotation = atan2(direction.x, direction.z); // XとZの成分からY軸の角度を取得

		// プレイヤーの回転を設定する（Y軸のみを使用）
		Vector3 newRotation;
		newRotation.x = 0.0f;  // ピッチは固定して水平を維持
		newRotation.y = yRotation;;  // ラジアンを度に変換
		newRotation.z = 0.0f;  // ロールも固定

		// Y軸の回転を反転させる
		//newRotation.y *= -1.0f;

		// プレイヤーの回転を設定
		player_->SetRotation(newRotation);

		// インデックスを次に進める（スムーズに進行）
		splineIndex_++;
		if (splineIndex_ >= spline_->GetSplinePoints().size()) {
			splineIndex_ = 0; // スプラインのループ
		}
	}
	else {
		splineIndex_ = 0; // スプラインのループ
	}








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
	const float interpolationFactor = 0.05f;
	switch (cameraMode_)
	{
	case CameraMode::FOLLOW:
	{
		currentCamera_->SplineFollowCamera(spline_->GetSplinePoints(), splineIndex_);

	}
	break;
	case CameraMode::TOP_DOWN:
	{
		Vector3 topDownPosition = Vector3(0.0f, 100.0f, 0.0f);
		currentCamera_->SetTopDownCamera(topDownPosition + player_->GetPosition());
	}
	break;
	case CameraMode::FPS:
	{
		// プレイヤーの位置と回転を取得してFPS視点にセット
		// 調整中。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
		currentCamera_->SetFPSCamera(player_->GetPosition(), player_->GetRotation());


	}
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

void GameScene::SpawnEnemyNearSpline()
{
	if (spline_->GetSplinePoints().empty()) return;  // スプラインポイントが空なら処理しない

	// スプライン上のランダムなポイントを選択
	size_t randomIndex = rand() % spline_->GetSplinePoints().size();
	Vector3 splinePoint = spline_->GetSplinePoints()[randomIndex];

	// ランダムなオフセットを追加してスプライン付近に敵をスポーン
	Vector3 offset = Vector3{ static_cast<float>(rand() % 20 - 5), 0.0f, static_cast<float>(rand() % 20 - 5) };
	Vector3 enemyPos = splinePoint + offset;

	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyPos);  // 敵の位置を設定
	enemies_.push_back(enemy);
}