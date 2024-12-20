#include "Player.h"
// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"
#include "Collision./GlobalVariables.h"
#include "Collision/CollisionTypeIdDef.h"

#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG




void Player::Initialize()
{
    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("cube.obj");

    // その他初期化
    input_ = Input::GetInstance();
    moveSpeed_ = { 0.25f, 0.25f , 0.25f };
    worldTransform_.Initialize();

    //worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

    //GlobalVariables* globalvariables = GlobalVariables::GetInstance();
    const char* groupName = "Player";
    // グループを追加
    GlobalVariables::GetInstance()->CreateGroup(groupName);

    // TypeIDの設定
    Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));
}

void Player::Update()
{
    Move();

    ShowCoordinatesImGui();

    UpdateWorldTransform();
}

void Player::Draw()
{
    if (isDrawEnabled_) {
        base_->Draw(worldTransform_);
    }
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
    ImGui::Begin("Player Editor");
    ImGui::Text("DrawCall");
    ImGui::Checkbox("Enable Draw", &isDrawEnabled_);
    // スケール
    ImGui::Text("Scale");
    float scale[3] = { worldTransform_.scale_.x, worldTransform_.scale_.y, worldTransform_.scale_.z };
    if (ImGui::SliderFloat3("Scale", scale, 0.1f, 10.0f, "%.2f"))
    {
        worldTransform_.scale_.x = (std::max)(0.1f, scale[0]); // スケールの下限を0.1に制限
        worldTransform_.scale_.y = (std::max)(0.1f, scale[1]);
        worldTransform_.scale_.z = (std::max)(0.1f, scale[2]);
    }

    // 回転
    ImGui::Text("Rotation");
    float rotation[3] = { worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z };
    if (ImGui::SliderFloat3("Rotation", rotation, -360.0f, 360.0f, "%.2f"))
    {
        worldTransform_.rotation_.x = rotation[0];
        worldTransform_.rotation_.y = rotation[1];
        worldTransform_.rotation_.z = rotation[2];
    }

    // 平行移動
    ImGui::Text("Translation");
    float translation[3] = { worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z };
    if (ImGui::SliderFloat3("Translation", translation, -100.0f, 100.0f, "%.2f"))
    {
        worldTransform_.translation_.x = translation[0];
        worldTransform_.translation_.y = translation[1];
        worldTransform_.translation_.z = translation[2];
    }

    ImGui::End();
#endif
   
}

void Player::OnCollision(Collider* other)
{
    // 衝突相手の種別IDを取得
    uint32_t typeID = other->GetTypeID();
    // 衝突相手が敵なら
    if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {

        isColliding_ = true;
    }
}

Vector3 Player::GetCenterPosition() const
{
    // ローカル座標でのオフセット
    const Vector3 offset = { 0.0f, 0.0f, 0.0f };
    // ワールド座標に変換
    Vector3 worldPos = TransformCoordinates(offset, worldTransform_.matWorld_);

    return worldPos;
}
