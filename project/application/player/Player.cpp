#include "Player.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <iostream>



void Player::Initailize()
{
    // .obj読み込み
    ModelManager::GetInstance()->LoadModel("player.obj");

    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("player.obj");

    // その他初期化
    input_ = Input::GetInstance();
    moveSpeed_ = { 0.5f, 0.5f , 0.5f };
    worldTransform_.Initialize();
}

void Player::Update()
{
    Move();
    ShowCoordinatesImGui(); 
    LastUpdate();
}

void Player::Draw()
{
    base_->Draw(worldTransform_);
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
    if (input_->IsPressMouse(0)) {
        worldTransform_.translation_.y += moveSpeed_.z;
    }

    // マウスの押下をチェック
    if (input_->IsPressMouse(1)) {
        worldTransform_.translation_.y -= moveSpeed_.z;
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
        if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
            worldTransform_.translation_.y += moveSpeed_.y;
        }

        // Bボタンでしゃがみ処理
        if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
            worldTransform_.translation_.y -= moveSpeed_.y;
        }
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
