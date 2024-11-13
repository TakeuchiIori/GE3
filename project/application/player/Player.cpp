#include "Player.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
#include <json.hpp>


void Player::Initailize()
{
    // .obj読み込み
    ModelManager::GetInstance()->LoadModel("Resources","player.obj");

    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("player.obj");

    // その他初期化
    input_ = Input::GetInstance();
    moveSpeed_ = { 0.5f, 0.5f , 0.5f };
    worldTransform_.Initialize();
    worldTransform_.translation_.z = -100.0f;
}

void Player::Update()
{
    Move();
    ShowCoordinatesImGui(); 
    UpdateWorldTransform();
}

void Player::Draw()
{
    base_->Draw(worldTransform_);
}

void Player::UpdateWorldTransform()
{
    // ワールドトランスフォームの更新
    worldTransform_.UpdateMatrix();
}

void Player::Move()
{
    // キーボードで移動
    MoveKey();
}

void Player::MoveKey()
{
    // 回転のスピード設定（回転速度を適宜設定）
    const float rotationSpeed = 0.05f;

    /*==============================================================
    
           矢印キーでカメラの回転（FPS視点の時にしか使わないほうが良い）

    ===============================================================*/

    // 上下の回転（ピッチ）の処理
    if (input_->PushKey(DIK_UP)) {
        worldTransform_.rotation_.x -= rotationSpeed;  // 上方向に回転
    }
    if (input_->PushKey(DIK_DOWN)) {
        worldTransform_.rotation_.x += rotationSpeed;  // 下方向に回転
    }

    // 左右回転（ヨー）の処理
    if (input_->PushKey(DIK_LEFT)) {
        worldTransform_.rotation_.y -= rotationSpeed;  // 左に回転
    }
    if (input_->PushKey(DIK_RIGHT)) {
        worldTransform_.rotation_.y += rotationSpeed;  // 右に回転
    }

    // 移動方向を計算（Y軸の回転に基づいて計算）
    Vector3 forwardDirection = {
        sinf(worldTransform_.rotation_.y), // Y軸の回転に応じた前方方向
        0.0f,
        cosf(worldTransform_.rotation_.y)
    };
    Vector3 rightDirection = {
        cosf(worldTransform_.rotation_.y),
        0.0f,
        -sinf(worldTransform_.rotation_.y)
    };

    // キーボード入力による移動
    if (input_->PushKey(DIK_W)) {
        // 前進（進行方向に沿って前方に移動）
        worldTransform_.translation_ += forwardDirection * moveSpeed_.z;
    }
    if (input_->PushKey(DIK_S)) {
        // 後退（進行方向に沿って後方に移動）
        worldTransform_.translation_ -= forwardDirection * moveSpeed_.z;
    }
    if (input_->PushKey(DIK_A)) {
        // 左に移動（進行方向に垂直な左方向に移動）
        worldTransform_.translation_ -= rightDirection * moveSpeed_.y;
    }
    if (input_->PushKey(DIK_D)) {
        // 右に移動（進行方向に垂直な右方向に移動）
        worldTransform_.translation_ += rightDirection * moveSpeed_.y;
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
