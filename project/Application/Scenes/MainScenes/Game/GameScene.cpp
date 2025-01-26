#include "GameScene.h"
// Engine
#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "PipelineManager/SkinningManager.h"
#include "Loaders/Model/Model.h"
#include "Collision/CollisionManager.h"

#include <cstdlib>
#include <ctime>

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include "LightManager/LightManager.h"
#include "Sprite/SpriteCommon.h"

/// <summary>
/// 初期化処理
/// </summary>
void GameScene::Initialize()
{
    srand(static_cast<unsigned int>(time(nullptr))); // 乱数シード設定
    // カメラの生成
    currentCamera_ = cameraManager_.AddCamera();
    Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());
    CollisionManager::GetInstance()->Initialize();
    // 線
    line_ = std::make_unique<Line>();
    line_->Initialize();
    line_->SetCamera(currentCamera_.get());

    boneLine_ = std::make_unique<Line>();
    boneLine_->Initialize();
    boneLine_->SetCamera(currentCamera_.get());


    //// コマンドパターン
    //inputHandler_ = std::make_unique<InputHandleMove>();
    //inputHandler_->AssignMoveFrontCommandPressKeyW();
    //inputHandler_->AssignMoveBehindCommandPressKeyS();
    //inputHandler_->AssignMoveRightCommandPressKeyD();
    //inputHandler_->AssignMoveLeftCommandPressKeyA();



    // 各オブジェクトの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
   

    // 敵
    //enemy_ = std::make_unique<Enemy>();
    //enemy_->Initialize();
    //enemy_->SetPlayer(player_.get());

    // 地面
    ground_ = std::make_unique<Ground>();
    ground_->Initialize();

    // test
    test_ = std::make_unique<Object3d>();
    test_->Initialize();
    //test_->SetModel("Animation_Node_00.gltf",true);
    test_->SetModel("ICo.obj");
    testWorldTransform_.Initialize();

    // アニメーション確認用
	animation_ = std::make_unique<Object3d>();
	animation_->Initialize();
	animation_->SetModel("AnimatedCube.gltf", true);
	animationTransform_.Initialize();


    //test_->SetLine(line_.get());

    // 初期カメラモード設定
    cameraMode_ = CameraMode::DEFAULT;

    // パーティクル
    std::string particleName = "Circle";
    ParticleManager::GetInstance()->SetCamera(currentCamera_.get());
    ParticleManager::GetInstance()->CreateParticleGroup(particleName, "Resources/images/circle.png");
    emitterPosition_ = Vector3{ 0.0f, 0.0f, 0.0f }; // エミッタの初期位置
    particleCount_ = 1;
    particleEmitter_[0] = std::make_unique<ParticleEmitter>(particleName, emitterPosition_, particleCount_);

}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update()
{
    CheckAllCollisions();
    CollisionManager::GetInstance()->UpdateWorldTransform();

    //if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
    //    SceneManager::GetInstance()->ChangeScene("TITLE");
    //}
    //iCommand_ = inputHandler_->HandleInput();
    //if (this->iCommand_) {
    //    iCommand_->Exec(*player_.get());
    //}




    // objの更新
    player_->Update();


   // enemy_->Update();
    ground_->Update();
    //test_->UpdateAnimation();
	animation_->UpdateAnimation();

    // カメラ更新
    UpdateCameraMode();
    UpdateCamera();

    // パーティクル更新
    ParticleManager::GetInstance()->Update();

    ChamgeLoadAnimation();
    // ShowImGui();
    particleEmitter_[0]->Update();

    // ワールドトランスフォーム更新
    testWorldTransform_.UpdateMatrix();
    animationTransform_.UpdateMatrix();
    cameraManager_.UpdateAllCameras();

    //=====================================================//
    /*                  これより下は触るな危険　　　　　　　   　*/
    //=====================================================//
   
    // ライティング
    LightManager::GetInstance()->ShowLightingEditor();

   
}


/// <summary>
/// 描画処理
/// </summary>
void GameScene::Draw()
{
#pragma region 演出描画
    //ParticleManager::GetInstance()->Draw();



#pragma endregion
  
#pragma region 2Dスプライト描画
    SpriteCommon::GetInstance()->DrawPreference();
    /// <summary>
    /// ここから描画可能です
    /// </summary>
    //player_->DrawSprite();
   

#pragma endregion

#pragma region 3Dオブジェクト描画
    Object3dCommon::GetInstance()->DrawPreference();
    LightManager::GetInstance()->SetCommandList();
    /// <summary>
    /// ここから描画可能です
    /// </summary>
    CollisionManager::GetInstance()->Draw();
    //player_->Draw();
    //enemy_->Draw();
    // その他の描画処理

    ground_->Draw();
    //line_->UpdateVertices(start_, end_);
  
    //line_->DrawLine();
    if (isTestDraw_) {
        test_->Draw(testWorldTransform_);
    }
    if (isAnimationDraw_) {
        animation_->Draw(animationTransform_);
    }
#pragma endregion

#pragma region 骨付きアニメーション描画
    SkinningManager::GetInstance()->DrawPreference();
    LightManager::GetInstance()->SetCommandList();
    /// <summary>
    /// ここから描画可能です
    /// </summary>

	

    // 骨描画
    //if (animation_&& animation_->GetModel()->GetSkeleton().joints.size() > 0) {
    //    animation_->DrawSkeleton(animation_->GetModel()->GetSkeleton(), *boneLine_);
    //    boneLine_->DrawLine();
    //}

   

#pragma endregion


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
    //if (ImGui::Button("Follow Camera")) {
    //    cameraMode_ = CameraMode::FOLLOW;
    //}
    if (ImGui::Button("Top-Down Camera")) {
        cameraMode_ = CameraMode::TOP_DOWN;
    }
    //if (ImGui::Button("FPS Camera")) {
    //    cameraMode_ = CameraMode::FPS;
    //}
    ImGui::End();
#endif
}

void GameScene::UpdateCamera()
{
    switch (cameraMode_)
    {
    case CameraMode::DEFAULT:
    {
        currentCamera_->DefaultCamera();
    }
    break;
    case CameraMode::FOLLOW:
    {
        Vector3 playerPos = player_->GetPosition();
        //currentCamera_->FollowCamera(playerPos);
    }
    break;
    case CameraMode::TOP_DOWN:
    {
        Vector3 topDownPosition = Vector3(0.0f, 50.0f, 0.0f);
        currentCamera_->SetTopDownCamera(topDownPosition + Vector3{0.0f,0.0f,0.0f});
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
#ifdef _DEBUG
    ImGui::Begin("FPS");
    ImGui::Text("FPS:%.1f", ImGui::GetIO().Framerate);
    ImGui::End();
    ImGui::Begin("Emitter");
    ImGui::DragFloat3("Emitter Position", &emitterPosition_.x, 0.1f);
    particleEmitter_[0]->SetPosition(emitterPosition_);

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
    particleEmitter_[0]->SetCount(particleCount_);


    ImGui::End();
#endif // _DEBUG
}

void GameScene::CheckAllCollisions() {

    // 衝突マネージャーのリセット
    CollisionManager::GetInstance()->Reset();

    // コライダーをリストに登録
    CollisionManager::GetInstance()->AddCollider(player_.get());

    // コライダーリストに登録
    CollisionManager::GetInstance()->AddCollider(player_->GetPlayerWeapon());

    // 敵全てについて
    //CollisionManager::GetInstance()->AddCollider(enemy_.get());

    // 衝突判定と応答
    CollisionManager::GetInstance()->CheckAllCollisions();

}

void GameScene::ChamgeLoadAnimation()
{
    static std::string currentAnimationFile = "None"; // 現在のアニメーションファイル名を保持

    ImGui::Begin("Animation");

    ImGui::Text("Animation_Node");

    if (ImGui::Button("Animation_Node_00")) {
        currentAnimationFile = "Animation_Node_00.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_Node_01")) {
        currentAnimationFile = "Animation_Node_01.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_Node_02")) {
        currentAnimationFile = "Animation_Node_02.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_Node_03")) {
        currentAnimationFile = "Animation_Node_03.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_Node_04")) {
        currentAnimationFile = "Animation_Node_04.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_Node_05")) {
        currentAnimationFile = "Animation_Node_05.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }


    if (ImGui::Button("Animation_NodeMisc_00")) {
        currentAnimationFile = "Animation_NodeMisc_00.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_NodeMisc_01")) {
        currentAnimationFile = "Animation_NodeMisc_01.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_NodeMisc_02")) {
        currentAnimationFile = "Animation_NodeMisc_02.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }

    if (ImGui::Button("Animation_NodeMisc_03")) {
        currentAnimationFile = "Animation_NodeMisc_03.gltf";
        animation_->SetModel(currentAnimationFile.c_str(), true);
    }


    // 現在のアニメーションファイル名を表示
    ImGui::Text("Now Playing: %s", currentAnimationFile.c_str());

    ImGui::Checkbox("AnimationDraw", &isAnimationDraw_);
    ImGui::Checkbox("TestDraw", &isTestDraw_);
    ImGui::End();

}


