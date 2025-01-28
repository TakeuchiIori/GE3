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
    sceneCamera_ = cameraManager_.AddCamera();
    Object3dCommon::GetInstance()->SetDefaultCamera(sceneCamera_.get());
    CollisionManager::GetInstance()->Initialize();
    // 線
    line_ = std::make_unique<Line>();
    line_->Initialize();
    line_->SetCamera(sceneCamera_.get());

    boneLine_ = std::make_unique<Line>();
    boneLine_->Initialize();
    boneLine_->SetCamera(sceneCamera_.get());


    //// コマンドパターン
    //inputHandler_ = std::make_unique<InputHandleMove>();

    //inputHandler_->AssignMoveFrontCommandPressKeyW();
    //inputHandler_->AssignMoveBehindCommandPressKeyS();
    //inputHandler_->AssignMoveRightCommandPressKeyD();
    //inputHandler_->AssignMoveLeftCommandPressKeyA();


    
	followCamera_.Initialize();
    // 各オブジェクトの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
    followCamera_.SetTarget(player_.get()->GetWorldTransform());
    player_->SetCamera(sceneCamera_.get());
    
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
    test_->SetModel("walk.gltf",true);
    //test_->SetModel("sneakWalk.gltf", true);
    testWorldTransform_.Initialize();
    //test_->SetLine(line_.get());

    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;

    // パーティクル
    const std::string particleName = "Circle";
    ParticleManager::GetInstance()->SetCamera(sceneCamera_.get());
    ParticleManager::GetInstance()->CreateParticleGroup(particleName, "Resources/images/circle.png");
    emitterPosition_ = Vector3{ 0.0f, 0.0f, 0.0f }; // エミッタの初期位置
    particleCount_ = 1;
    particleEmitter_[0] = std::make_unique<ParticleEmitter>(particleName, emitterPosition_, particleCount_);

    //// パーティクルグループ名を指定
    //const std::string particleGroupName = "PlayerWeaponEffect";
    //weaponPos = player_->GetPosition();
    //ParticleManager::GetInstance()->CreateParticleGroup(particleGroupName, "Resources/images/circle.png");
    //particleEmitter_[1] = std::make_unique<ParticleEmitter>(particleGroupName, weaponPos, 5);



    phase_ = Phase::kFadeIn;
    fade_ = std::make_unique<Fade>();
    fade_->Initialize("Resources/white.png");
    fade_->Start(Fade::Status::FadeIn,2.0f);
}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update()
{


    ChangePahse();
    
   // if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
   //     SceneManager::GetInstance()->ChangeScene("TITLE");
   // }
   // 
   // //iCommand_ = inputHandler_->HandleInput();

   // //if (this->iCommand_) {
   // //    iCommand_->Exec(*player_.get());
   // //}

   // CheckAllCollisions();
   // CollisionManager::GetInstance()->UpdateWorldTransform();
   // // スポーンタイマーを更新
   // spawnTimer_ += 1.0f / 60.0f; // フレーム時間を加算
   // if (spawnTimer_ >= spawnInterval_) {
   //     SpawnEnemy();
   //     spawnTimer_ = 0.0f;
   // }

   // // 各敵を更新
   // for (auto it = enemies_.begin(); it != enemies_.end();) {
   //     auto& enemy = *it;
   //     enemy->Update();
   //     if (!enemy->IsActive()) {
   //         it = enemies_.erase(it); // Remove and release memory for inactive enemies.
   //     }
   //     else {
   //         ++it;
   //     }
   // }

   // // objの更新
   // player_->Update();


   //// enemy_->Update();
   // ground_->Update();
   // test_->UpdateAnimation();

   // // カメラ更新
   // UpdateCameraMode();
   // UpdateCamera();

   // // パーティクル更新
   // ParticleManager::GetInstance()->Update();
   //// ParticleManager::GetInstance()->UpdateParticlePlayerWeapon(weaponPos);
   // ShowImGui();
   // particleEmitter_[0]->Update();
   //// particleEmitter_[1]->Update();
  

   // // ワールドトランスフォーム更新
   // testWorldTransform_.UpdateMatrix();
   // cameraManager_.UpdateAllCameras();

   // //=====================================================//
   // /*                  これより下は触るな危険　　　　　　　   　*/
   // //=====================================================//
   //
   // // ライティング
   // LightManager::GetInstance()->ShowLightingEditor();

   
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
    
    // フェードイン&&フェードアウト中はフェードの描画
    if (phase_ == Phase::kFadeIn || phase_ == Phase::kFadeOut) {
        fade_->Draw();
    }

#pragma endregion

#pragma region 3Dオブジェクト描画
    Object3dCommon::GetInstance()->DrawPreference();
    LightManager::GetInstance()->SetCommandList();
    /// <summary>
    /// ここから描画可能です
    /// </summary>
    CollisionManager::GetInstance()->Draw();
    player_->Draw();
  //  enemy_->Draw();
        // その他の描画処理
    for (auto& enemy : enemies_) {
        enemy->Draw();
    }
    ground_->Draw();
    //line_->UpdateVertices(start_, end_);
  
    //line_->DrawLine();
   
#pragma endregion

#pragma region 骨付きアニメーション描画
    SkinningManager::GetInstance()->DrawPreference();
    LightManager::GetInstance()->SetCommandList();
    /// <summary>
    /// ここから描画可能です
    /// </summary>

   // test_->Draw(testWorldTransform_);

    // 骨描画
    //if (test_ && test_->GetModel()->GetSkeleton().joints.size() > 0) {
    //    test_->DrawSkeleton(test_->GetModel()->GetSkeleton(), *boneLine_);
    //    boneLine_->DrawLine();
    //}

   

#pragma endregion


}

/// <summary>
/// 解放処理
/// </summary>
void GameScene::Finalize()
{
    cameraManager_.RemoveCamera(sceneCamera_);
}

void GameScene::ChangePahse()
{
    switch (phase_)
    {
    case Phase::kFadeIn:


        //if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        //    SceneManager::GetInstance()->ChangeScene("TITLE");
        //}

        //iCommand_ = inputHandler_->HandleInput();

        //if (this->iCommand_) {
        //    iCommand_->Exec(*player_.get());
        //}

        //CheckAllCollisions();
        //CollisionManager::GetInstance()->UpdateWorldTransform();
        // スポーンタイマーを更新
        spawnTimer_ += 1.0f / 60.0f; // フレーム時間を加算
        if (spawnTimer_ >= spawnInterval_) {
            SpawnEnemy();
            spawnTimer_ = 0.0f;
        }

        // 各敵を更新
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            enemy->Update();
            if (!enemy->IsActive()) {
                it = enemies_.erase(it); // Remove and release memory for inactive enemies.
            }
            else {
                ++it;
            }
        }

        // objの更新
        player_->Update();


        ground_->Update();
        test_->UpdateAnimation();

        // カメラ更新
        UpdateCameraMode();
        UpdateCamera();

        // パーティクル更新
        //ParticleManager::GetInstance()->Update();
        // ParticleManager::GetInstance()->UpdateParticlePlayerWeapon(weaponPos);
        ShowImGui();
        //particleEmitter_[0]->Update();
        // particleEmitter_[1]->Update();


         // ワールドトランスフォーム更新
        testWorldTransform_.UpdateMatrix();
        cameraManager_.UpdateAllCameras();

        //=====================================================//
        /*                  これより下は触るな危険　　　　　　　   　*/
        //=====================================================//

        // ライティング
        LightManager::GetInstance()->ShowLightingEditor();




        fade_->Update();
        if (fade_->IsFinished()) {
            phase_ = Phase::kPlay;
        }

        break;
    case Phase::kPlay:


        if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
            phase_ = Phase::kFadeOut;
            fade_->Start(Fade::Status::FadeOut, 2.0f);
        }

        //iCommand_ = inputHandler_->HandleInput();

        //if (this->iCommand_) {
        //    iCommand_->Exec(*player_.get());
        //}

        CheckAllCollisions();
        CollisionManager::GetInstance()->UpdateWorldTransform();
        // スポーンタイマーを更新
        spawnTimer_ += 1.0f / 60.0f; // フレーム時間を加算
        if (spawnTimer_ >= spawnInterval_) {
            SpawnEnemy();
            spawnTimer_ = 0.0f;
        }

        // 各敵を更新
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            auto& enemy = *it;
            enemy->Update();
            if (!enemy->IsActive()) {
                it = enemies_.erase(it); // Remove and release memory for inactive enemies.
            }
            else {
                ++it;
            }
        }

        // objの更新
        player_->Update();
        

        // enemy_->Update();
        ground_->Update();
        test_->UpdateAnimation();

        // カメラ更新
        UpdateCameraMode();
        UpdateCamera();

        // パーティクル更新
        ParticleManager::GetInstance()->Update();
        // ParticleManager::GetInstance()->UpdateParticlePlayerWeapon(weaponPos);
        ShowImGui();
        particleEmitter_[0]->Update();
        // particleEmitter_[1]->Update();


         // ワールドトランスフォーム更新
        testWorldTransform_.UpdateMatrix();
        cameraManager_.UpdateAllCameras();

        //=====================================================//
        /*                  これより下は触るな危険　　　　　　　   　*/
        //=====================================================//

        // ライティング
        LightManager::GetInstance()->ShowLightingEditor();



        break;
    case Phase::kDeath:




        break;
    case Phase::kFadeOut:
        fade_->Update();
        if (fade_->IsFinished()) {
            finished_ = true;
        }
        break;
    default:
        break;
    }


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
        sceneCamera_->DefaultCamera();
    }
    break;
    case CameraMode::FOLLOW:
    {
        
        followCamera_.Update();
		sceneCamera_->viewMatrix_ = followCamera_.matView_;
		sceneCamera_->transform_.translate = followCamera_.translate_;
		sceneCamera_->transform_.rotate = followCamera_.rotate_;

		sceneCamera_->UpdateMatrix();
    }
    break;
    case CameraMode::TOP_DOWN:
    {
        //Vector3 topDownPosition = Vector3(0.0f, 100.0f, 0.0f);
        //sceneCamera_->SetTopDownCamera(topDownPosition + player_->GetPosition());
        
        topDownCamera_.SetTarget(player_.get()->GetWorldTransform());
        topDownCamera_.Update();
		sceneCamera_->viewMatrix_ = topDownCamera_.matView_;
		sceneCamera_->transform_.translate = topDownCamera_.translate_;
		sceneCamera_->transform_.rotate = topDownCamera_.rotate_;

		sceneCamera_->UpdateMatrix();
    }
    break;
    case CameraMode::FPS:
    {

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
        // その他の描画処理
    for (auto& enemy : enemies_) {
        CollisionManager::GetInstance()->AddCollider(enemy.get());
    }
   // CollisionManager::GetInstance()->AddCollider(enemy_.get());

    // 衝突判定と応答
    CollisionManager::GetInstance()->CheckAllCollisions();

}

void GameScene::SpawnEnemy()
{
    // ランダムな位置を計算
    Vector3 playerPos = player_->GetPosition();
    float offsetX = (rand() % 100 / 100.0f) * spawnRange_ * 2 - spawnRange_;
    float offsetZ = (rand() % 100 / 100.0f) * spawnRange_ * 2 - spawnRange_;
    Vector3 spawnPos = playerPos + Vector3{ offsetX, 0.0f, offsetZ };

    // 新しい敵を生成
    auto newEnemy = std::make_unique<Enemy>();
    newEnemy->Initialize();
    newEnemy->SetPlayer(player_.get());
    newEnemy->SetPosition(spawnPos);

    // リストに追加
    enemies_.emplace_back(std::move(newEnemy));
}


