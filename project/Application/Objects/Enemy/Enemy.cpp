#include "Enemy.h"

// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"
#include "Collision./GlobalVariables.h"
#include "Collision/CollisionTypeIdDef.h"
#include "Player/Player.h"
#include "Particle/ParticleManager.h"

#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG
// 次のシリアルナンバー
uint32_t Enemy::nextSerialNumber_ = 0;
Enemy::Enemy() {
    // シリアルナンバーを振る
    serialNumber_ = nextSerialNumber_;
    // 番号の追加
    ++nextSerialNumber_;
}
void Enemy::Initialize(Camera* camera, const Vector3& pos)
{
    camera_ = camera;

    // OBject3dの初期化
    base_ = std::make_unique<Object3d>();
    base_->Initialize();
    base_->SetModel("needle_body.obj");
    //needle_Body
    // その他初期化
    input_ = Input::GetInstance();
    //moveSpeed_ = { 0.25f, 0.25f , 0.25f };
    worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
    worldTransform_.translation_.y = 1.0f;
   // worldTransform_.translation_.z = 25.0f;
    shadow_ = std::make_unique<Object3d>();
    shadow_->Initialize();
    shadow_->SetModel("Shadow.obj");

    WS_.Initialize();

    WS_.translation_.y = 0.1f;

    //GlobalVariables* globalvariables = GlobalVariables::GetInstance();
    const char* groupName = "Enemy";
    // グループを追加
   // GlobalVariables::GetInstance()->CreateGroup(groupName);
   // Collider::Initialize();
    // TypeIDの設定
    Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));


    particleEmitter_ = std::make_unique<ParticleEmitter>("Enemy", worldTransform_.translation_, 5);
    particleEmitter_->Initialize();
	
}

void Enemy::Update()
{
    // 死亡時は更新処理をスキップ
    if (!isActive_ || !isAlive_) {
        return;
    }


    CameraShake();

   
    Move();


    particleEmitter_->UpdateEmit("Enemy", worldTransform_.translation_, 5);
    

    if (isHit_) {
        base_->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });
    }
    else {
        base_->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    }
	isHit_ = false;

#ifdef _DEBUG
   ShowCoordinatesImGui();

#endif // _DEBUG

    worldTransform_.UpdateMatrix();
    WS_.UpdateMatrix();
}

void Enemy::Draw()
{
    if (!isActive_ || !isAlive_) {
        return;
    }
        base_->Draw(camera_,worldTransform_);
        shadow_->Draw(camera_,WS_);
  
    
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
    // 衝突相手が武器かプレイヤーなら
    if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {

        isHit_ = true;
		base_->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        hp_ -= 2;
        if (hp_ <= 0) {
            isAlive_ = false;
            isActive_ = false;  // 完全に無効化
        }
		isShake_ = true;



       


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

void Enemy::Move() {

    // プレイヤーの位置を取得
    Vector3 playerPos = player_->GetPosition();
    // プレイヤーへの方向ベクトルを計算
    Vector3 pos = playerPos - worldTransform_.translation_;
    float weaponRadius = 4.0f;

    // プレイヤーとの距離が一定以上なら近づく
    if (Length(pos) > weaponRadius + radius_) {
        // 方向の正規化
        pos = Normalize(pos);
        // プレイヤーの方向に移動
        worldTransform_.translation_ += pos * speed_;
    }

    // 高さは固定
    worldTransform_.translation_.y = 1.0f;

    // 影の位置更新
    WS_.translation_.x = worldTransform_.translation_.x;
    WS_.translation_.z = worldTransform_.translation_.z;
    WS_.translation_.y = 0.1f;

    // プレイヤーの方向を向く
    Vector3 lockOnPos = player_->GetPosition();
    Vector3 sub = lockOnPos - worldTransform_.translation_;
    worldTransform_.rotation_.y = std::atan2(sub.x, sub.z);
}

void Enemy::CameraShake()
{
    if (isShake_) {
        camera_->Shake(0.1f, Vector2{ -0.1f, -0.1f },Vector2{0.1f,0.1f});
		isShake_ = false;
    }
}

void Enemy::InitJson()
{
	jsonManager_ = new JsonManager("Enemy", "Resources./JSON");
	jsonManager_->Register("HP", &hp_);

}
