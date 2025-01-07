#include "Enemy.h"

// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"
#include "Collision./GlobalVariables.h"
#include "Collision/CollisionTypeIdDef.h"

#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG

void Enemy::Initialize()
{
    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("cube.obj");
    //needle_Body
    // その他初期化
    input_ = Input::GetInstance();
    moveSpeed_ = { 0.25f, 0.25f , 0.25f };
    worldTransform_.Initialize();
    worldTransform_.translation_.y = 1.0f;
    //GlobalVariables* globalvariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    // グループを追加
   // GlobalVariables::GetInstance()->CreateGroup(groupName);
   // Collider::Initialize();
    // TypeIDの設定
    Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
}

void Enemy::Update()
{
    Move();

    ShowCoordinatesImGui();

    worldTransform_.UpdateMatrix();
}

void Enemy::Draw()
{
    if (isDrawEnabled_) {
        base_->Draw(worldTransform_);
    }
}

void Enemy::ShowCoordinatesImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Enemy");
    ImGui::Checkbox("Enable Draw", &isDrawEnabled_);

    // スケール
    //ImGui::Text("Scale");
    //float scale[3] = { worldTransform_.scale_.x, worldTransform_.scale_.y, worldTransform_.scale_.z };
    //if (ImGui::SliderFloat3("Scale", scale, 0.1f, 10.0f, "%.2f")) {
    //    worldTransform_.scale_.x = std::max(0.1f, scale[0]);
    //    worldTransform_.scale_.y = std::max(0.1f, scale[1]);
    //    worldTransform_.scale_.z = std::max(0.1f, scale[2]);
    //}

    // 回転
    ImGui::Text("Rotation");
    float rotation[3] = { worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z };
    if (ImGui::SliderFloat3("Rotation", rotation, -360.0f, 360.0f, "%.2f")) {
        worldTransform_.rotation_.x = rotation[0];
        worldTransform_.rotation_.y = rotation[1];
        worldTransform_.rotation_.z = rotation[2];
    }

    // 平行移動
    ImGui::Text("Translation");
    float translation[3] = { worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z };
    if (ImGui::SliderFloat3("Translation", translation, -100.0f, 100.0f, "%.2f")) {
        worldTransform_.translation_.x = translation[0];
        worldTransform_.translation_.y = translation[1];
        worldTransform_.translation_.z = translation[2];
    }

    ImGui::End();
#endif // _DEBUG

}

void Enemy::OnCollision(Collider* other)
{
    // 衝突相手の種別IDを取得
    uint32_t typeID = other->GetTypeID();
    // 衝突相手が敵なら
    if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) || typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

       isColliding_ = true;
    }

}

Vector3 Enemy::GetCenterPosition() const
{
    // ローカル座標でのオフセット
    const Vector3 offset = { 0.0f, 0.0f, 0.0f };
    // ワールド座標に変換
    Vector3 worldPos = TransformCoordinates(offset, worldTransform_.matWorld_);

    return worldPos;
}

Matrix4x4 Enemy::GetWorldMatrix() const
{
    return worldTransform_.matWorld_;
}

void Enemy::Move()
{
    // 衝突中フラグが立っている場合は非表示に
    if (isColliding_) {
        isDrawEnabled_ = false; // 描画を無効に
    }
    else {
        isDrawEnabled_ = true; // 描画を有効に
    }
    // 衝突状態をリセット
    isColliding_ = false; // 毎フレーム初期化
}
