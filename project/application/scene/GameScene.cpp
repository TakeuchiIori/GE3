#include "GameScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ParticleManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG

/// <summary>
/// 初期化処理
/// </summary>
void GameScene::Initialize()
{
    // モデル読み込み
    ModelManager::GetInstance()->LoadModel("float_body.obj");

    // カメラの生成
    currentCamera_ = cameraManager_.AddCamera();
    Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());
   
    // 各オブジェクトの初期化
    player_ = std::make_unique<Player>();
    player_->Initailize();

    test_ = std::make_unique<Object3d>();
    test_->Initialize();
    test_->SetModel("float_body.obj");
    testWorldTransform_.Initialize();
    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;

    // パーティクル
    std::string particleName = "Circle";
    ParticleManager::GetInstance()->SetCamera(currentCamera_.get());
    ParticleManager::GetInstance()->CreateParticleGroup(particleName, "Resources/images/circle.png");
    particleEmitter_ = std::make_unique<ParticleEmitter>(particleName,Vector3{1.0f,1.0f,1.0f},10);
    
}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update()
{
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
    player_->Update();
    UpdateCameraMode();
    UpdateCamera();
    ParticleManager::GetInstance()->Update();
    particleEmitter_->Update();
    particleEmitter_->Emit();
    // ワールドトランスフォーム更新
    testWorldTransform_.TransferMatrix();
    cameraManager_.UpdateAllCameras();
}


/// <summary>
/// 描画処理
/// </summary>
void GameScene::Draw()
{
    // 描画準備
    PrepareDraw();
    player_->Draw();
    test_->Draw(testWorldTransform_);
    ParticleManager::GetInstance()->Draw();
}

/// <summary>
/// 解放処理
/// </summary>
void GameScene::Finalize()
{
    cameraManager_.RemoveCamera(currentCamera_);
}

void GameScene::UpdateCameraMode()
{
#ifdef _DEBUG
    ImGui::Begin("Camera Mode");
    if (ImGui::Button("DEFAULT Camera")) {
        cameraMode_ = CameraMode::DEFAULT;
    }
    if (ImGui::Button("Follow Camera")) {
        cameraMode_ = CameraMode::FOLLOW;
    }
    if (ImGui::Button("Top-Down Camera")) {
        cameraMode_ = CameraMode::TOP_DOWN;
    }
    ImGui::End();
#endif
}

void GameScene::UpdateCamera()
{
    switch (cameraMode_)
    {
    case CameraMode::DEFAULT:
    {
        currentCamera_->ResetToOrigin();
    }
    break;
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
    default:
        break;
    }
}

void GameScene::PrepareDraw()
{
    Object3dCommon::GetInstance()->DrawPreference();
    SpriteCommon::GetInstance()->DrawPreference();
}
