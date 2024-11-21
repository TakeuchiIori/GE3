#include "Enemy.h"
#include "ModelManager.h"
#include "collider/GlobalVariables.h"
void Enemy::Initialize()
{
    // .obj読み込み
    ModelManager::GetInstance()->LoadModel("Resources./Objects./player", "player.obj");

    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("player.obj");

    // その他初期化
    worldTransform_.Initialize();

    //GlobalVariables* globalvariables = GlobalVariables::GetInstance();
    //const char* groupName = "Enemy";
    //// グループを追加
    //GlobalVariables::GetInstance()->CreateGroup(groupName);


  

    // TypeIDの設定
    Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
}

void Enemy::Update()
{

    worldTransform_.UpdateMatrix();
}

void Enemy::Draw()
{
    base_->Draw(worldTransform_);
}

void Enemy::OnCollision(Collider* other)
{
    // 衝突相手の種別IDを取得
    uint32_t typeID = other->GetTypeID();
    // 衝突相手が敵なら
    if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {

       // worldTransform_.rotation_.y = 100.0f;
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
