#include "TitleScene.h"
// Engine
#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include "LightManager/LightManager.h"
#include "Sprite/SpriteCommon.h"
/// <summary>
/// 初期化処理
/// </summary>
void TitleScene::Initialize()
{
    // カメラの生成
    currentCamera_ = cameraManager_.AddCamera();
    Object3dCommon::GetInstance()->SetDefaultCamera(currentCamera_.get());

    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;


    // 各オブジェクトの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize(currentCamera_.get());

    sprite_ = std::make_unique<Sprite>();
    sprite_->Initialize("Resources/Textures/KoboTitle.png");
    sprite_->SetSize(Vector2{ 1280.0f,720.0f });
    sprite_->SetTextureSize(Vector2{ 1280,720 });


    fade_ = std::make_unique<Fade>();
    fade_->Initialize("Resources/images/white.png");
    fade_->Start(Fade::Status::FadeIn, 2.0f);

}

/// <summary>
/// 更新処理
/// </summary>
void TitleScene::Update()
{
    sprite_->Update();
    fade_->Update();
    GhangePhase();



}


/// <summary>
/// 描画処理
/// </summary>
void TitleScene::Draw()
{
#pragma region 演出描画
    ParticleManager::GetInstance()->Draw();



#pragma endregion
#pragma region 2Dスプライト描画
    SpriteCommon::GetInstance()->DrawPreference();
    /// <summary>
    /// ここから描画可能です
    /// </summary>
    /// 

    sprite_->Draw();
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

    player_->Draw();


#pragma endregion


}


void TitleScene::GhangePhase()
{
    switch (phase_)
    {
    case TitleScene::Phase::kFadeIn:
        if (fade_->IsFinished()) {
            phase_ = Phase::kMain;
        }

        break;
    case TitleScene::Phase::kMain:
        if (Input::GetInstance()->IsPadPressed(0,GamePadButton::A)) {
            phase_ = Phase::kFadeOut;
            fade_->Start(Fade::Status::FadeOut, 2.0f);
        }


        //// プレイヤーの更新
        //player_->Update();

        //// カメラ更新
        //UpdateCameraMode();
        //UpdateCamera();






        //cameraManager_.UpdateAllCameras();


        //// ライティング
        //LightManager::GetInstance()->ShowLightingEditor();


        break;
    case TitleScene::Phase::kFadeOut:
        if (fade_->IsFinished()) {
            SceneManager::GetInstance()->ChangeScene("Game");
        }


        break;
    default:
        break;
    }
}


/// <summary>
/// 解放処理
/// </summary>
void TitleScene::Finalize()
{
    cameraManager_.RemoveCamera(currentCamera_);
}


void TitleScene::UpdateCameraMode()
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

void TitleScene::UpdateCamera()
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

    }
    break;
    case CameraMode::TOP_DOWN:
    {

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



