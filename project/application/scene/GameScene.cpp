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
    // カメラの生成
    currentCamera_ = cameraManager_.AddCamera();
    Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());
   
    // 各オブジェクトの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();

    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize();

    // test
    test_ = std::make_unique<Object3d>();
    test_->Initialize();
    test_->SetModel("AnimatedCube.gltf",true);
    testWorldTransform_.Initialize();

    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;

    // パーティクル
    std::string particleName = "Circle";
    ParticleManager::GetInstance()->SetCamera(currentCamera_.get());
    ParticleManager::GetInstance()->CreateParticleGroup(particleName, "Resources/images/circle.png");
    emitterPosition_ = Vector3{ 0.0f, 0.0f, 0.0f }; // エミッタの初期位置
    particleCount_ = 1;
    particleEmitter_ = std::make_unique<ParticleEmitter>(particleName, emitterPosition_, particleCount_);

}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update()
{
    
    //if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
    //    SceneManager::GetInstance()->ChangeScene("TITLE");
    //}
    // プレイヤーの更新
    player_->Update();
    enemy_->Update();
    test_->Update();
    // カメラ更新
    UpdateCameraMode();
    UpdateCamera();

    // パーティクル更新
    ParticleManager::GetInstance()->Update();
    //particleEmitter_->SetPosition(emitterPosition_); // 更新した位置をエミッタに反映
    ShowImGui();
    particleEmitter_->Update();
   
    //test_->MaterialByImGui();

    // ワールドトランスフォーム更新
    testWorldTransform_.UpdateMatrix();
    cameraManager_.UpdateAllCameras();

    LightManager::GetInstance()->ShowLightingEditor();

   
}


/// <summary>
/// 描画処理
/// </summary>
void GameScene::Draw()
{
    //================== 2D ==================//
    SpriteCommon::GetInstance()->DrawPreference();
    ParticleManager::GetInstance()->Draw();
 
    //================== 3D ==================//
    Object3dCommon::GetInstance()->DrawPreference();

    //================== ライティング ==================//
    LightManager::GetInstance()->SetCommandList();
   
    player_->Draw();
    //enemy_->Draw();
    test_->Draw(testWorldTransform_);
  

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
    case CameraMode::FPS:
    {
        Vector3 playerPos = player_->GetPosition();
        currentCamera_->SetFPSCamera(playerPos, player_->GetRotation());
    }
    break;

    default:
        break;
    }
}

void GameScene::ShowImGui()
{
    ImGui::Begin("Emitter");
    ImGui::DragFloat3("Emitter Position", &emitterPosition_.x, 0.1f);
    particleEmitter_->SetPosition(emitterPosition_);

    // パーティクル数の表示と調整
    ImGui::Text("Particle Count: %.0d", particleCount_); // 現在のパーティクル数を表示
    if (ImGui::Button("Increase Count")) {
        particleCount_ += 1; // パーティクル数を増加
    }
    if (ImGui::Button("Decrease Count")) {
        if (particleCount_ > 1) { // パーティクル数が1未満にならないように制限
            particleCount_ -= 1;
        }
    }
    particleEmitter_->SetCount(particleCount_);


    ImGui::End();
}
