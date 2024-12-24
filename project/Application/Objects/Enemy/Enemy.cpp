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

    // その他初期化
    input_ = Input::GetInstance();
    moveSpeed_ = { 0.25f, 0.25f , 0.25f };
    worldTransform_.Initialize();

    //GlobalVariables* globalvariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    // グループを追加
    GlobalVariables::GetInstance()->CreateGroup(groupName);
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
}

void Enemy::OnCollision(Collider* other)
{
    // 衝突相手の種別IDを取得
    uint32_t typeID = other->GetTypeID();
    // 衝突相手が敵なら
    if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {

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

void Enemy::Move()
{
}
