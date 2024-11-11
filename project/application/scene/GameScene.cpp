#include "GameScene.h"
#include "SceneManager.h"
#include "Input.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
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

    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize(Vector3{0,0,-12});


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
    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
       // SceneManager::GetInstance()->ChangeScene("TITLE");
    }
    // 各オブジェクトの更新
    player_->Update();
    spline_->Update();
    enemy_->Update();

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
    enemy_->Draw();
    test_->Draw(testWorldTransform_);
    spline_->Draw();
}

void GameScene::FinalizeCamera()
{
    cameraManager_.RemoveCamera(currentCamera_);
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
