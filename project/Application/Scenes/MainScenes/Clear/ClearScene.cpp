#include "ClearScene.h"

#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "Sprite/SpriteCommon.h"

void ClearScene::Initialize()
{
    sprite_ = std::make_unique<Sprite>();
    sprite_->Initialize("Resources/Textures/KoboClear.png");
    sprite_->SetSize(Vector2{ 1280.0f,720.0f });
    sprite_->SetTextureSize(Vector2{ 1280,720 });


    fade_ = std::make_unique<Fade>();
    fade_->Initialize("Resources/images/white.png");
    fade_->Start(Fade::Status::FadeIn, 2.0f);
}

void ClearScene::Finalize()
{
}

void ClearScene::Update()
{
    sprite_->Update();
    fade_->Update();
    GhangePhase();
}

void ClearScene::Draw()
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
    /// <summary>
    /// ここから描画可能です
    /// </summary>



#pragma endregion

}

void ClearScene::GhangePhase()
{
    switch (phase_)
    {
    case ClearScene::Phase::kFadeIn:
        if (fade_->IsFinished()) {
            phase_ = Phase::kMain;
        }

        break;
    case ClearScene::Phase::kMain:
        if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
            phase_ = Phase::kFadeOut;
            fade_->Start(Fade::Status::FadeOut, 2.0f);
        }


        break;
    case ClearScene::Phase::kFadeOut:
        if (fade_->IsFinished()) {
            SceneManager::GetInstance()->ChangeScene("Title");
        }


        break;
    default:
        break;
    }
}
