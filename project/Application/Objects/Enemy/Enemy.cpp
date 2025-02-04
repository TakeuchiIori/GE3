#include "Enemy.h"

// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"
#include "Collision./GlobalVariables.h"
#include "Collision/CollisionTypeIdDef.h"
#include "Player/Player.h"
#include "Particle/ParticleManager.h"
#include <Systems/GameTime/HitStop.h>
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

    isActive_ = true;
    isAlive_ = true;

    particleEmitter_ = std::make_unique<ParticleEmitter>("Enemy", worldTransform_.translation_, 5);
    particleEmitter_->Initialize();
	
	timeID_ = "Enemy : " + std::to_string(serialNumber_);
	gameTime_ = GameTime::GetInstance();
	gameTime_->RegisterObject(timeID_);
}

void Enemy::Update()
{
    // 死亡時は更新処理をスキップ
    if (!isActive_ || !isAlive_) {
        return;
    }


    CameraShake();

   
    Move();


   
    

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
   particleEmitter_->UpdateEmit("Enemy", worldTransform_.translation_, 3);
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
	ImGui::DragFloat("deltaTime_", &deltaTime_, 0.01f, 0.0f, 10.0f);


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


        
        HitStop::GetInstance()->Start(timeID_, HitStop::HitStopType::Heavy);
        HitStop::GetInstance()->Start("Player", HitStop::HitStopType::Heavy);
        //HitStop::GetInstance()->Start(timeID_, 0.1f);
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
    // デルタタイムの取得と制限
    deltaTime_ = gameTime_->GetDeltaTime(timeID_);
    // ヒットストップ中は動きを停止
    if (deltaTime_ <= 0.001f) {
        return;
    }

    // プレイヤーへの方向ベクトルを計算
    Vector3 toPlayer = player_->GetWorldPosition() - worldTransform_.translation_;
    float distanceToPlayer = Length(toPlayer);

    // プレイヤーとの最小距離（これ以上近づかない）
    const float minDistance = radius_ + weaponRadius_;  // 自分の半径 + 武器の半径
    // プレイヤーとの最大距離（この距離以上離れると追跡を開始）
    const float maxDistance = 20.0f;  // 追跡を開始する距離

    // プレイヤーが追跡範囲内にいる場合
    if (distanceToPlayer > minDistance && distanceToPlayer < maxDistance) {
        // 移動方向の正規化
        Vector3 moveDirection = Normalize(toPlayer);

        // 現在の速度を計算（メートル/秒）
        Vector3 velocity = moveDirection * speedPerSecond_ * deltaTime_;

        // 移動速度に上限を設定
        float currentSpeed = Length(velocity);
        if (currentSpeed > maxSpeed_ * deltaTime_) {
            velocity = Normalize(velocity) * maxSpeed_ * deltaTime_;
        }

        // 位置の更新
        worldTransform_.translation_ += velocity;

        // プレイヤーとの最小距離を保持
        if (distanceToPlayer < minDistance) {
            Vector3 pushBackDir = Normalize(toPlayer);
            worldTransform_.translation_ = player_->GetWorldPosition() - (pushBackDir * minDistance);
        }
    }

    // 高さは固定
    worldTransform_.translation_.y = 1.0f;

    // 影の位置更新
    WS_.translation_.x = worldTransform_.translation_.x;
    WS_.translation_.z = worldTransform_.translation_.z;
    WS_.translation_.y = 0.1f;

    // プレイヤーの方向を向く（滑らかな回転に変更）
    Vector3 targetDirection = player_->GetWorldPosition() - worldTransform_.translation_;
    float targetRotationY = std::atan2(targetDirection.x, targetDirection.z);

    // 現在の回転から目標の回転まで補間
    float currentRotationY = worldTransform_.rotation_.y;
    float rotationDiff = targetRotationY - currentRotationY;

    // 回転角度を-πからπの範囲に正規化
    while (rotationDiff > std::numbers::pi_v<float>) rotationDiff -= 2 * std::numbers::pi_v<float>;
    while (rotationDiff < -std::numbers::pi_v<float>) rotationDiff += 2 * std::numbers::pi_v<float>;

    // 滑らかな回転
    worldTransform_.rotation_.y += rotationDiff * rotationSpeed_ * deltaTime_;
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
