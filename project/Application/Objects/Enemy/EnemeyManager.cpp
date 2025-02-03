#include "EnemeyManager.h"
#include <random>

void EnemeyManager::Initialize(Camera* camera) {
    camera_ = camera;
}

void EnemeyManager::Update() {
    // スポーンタイマーの更新
    spawnTimer_ += 1.0f / 60.0f;
    if (spawnTimer_ >= spawnInterval_ && enemies_.size() < maxEnemyCount_) {
        SpawnEnemy();
        spawnTimer_ = 0.0f;
    }

    // 安全な敵の更新
    if (!enemies_.empty()) {  // 空チェックを追加
        for (auto it = enemies_.begin(); it != enemies_.end();) {
            if (it != enemies_.end()) {  // 追加のチェック
                auto& enemy = *it;
                enemy->Update();

                // 敵が死んでいたら削除
                if (!enemy->IsActive()) {
                    it = enemies_.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
}

void EnemeyManager::Draw() {
    for (auto& enemy : enemies_) {
        enemy->Draw();
    }
}

void EnemeyManager::SpawnEnemy() {
    if (!player_) return;

    // 乱数生成
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-spawnRange_, spawnRange_);

    // プレイヤーの位置を基準にランダムな位置を計算
    Vector3 playerPos = player_->GetPosition();
    Vector3 spawnPos = {
        playerPos.x + dist(gen),
        1.0f,  // Y座標は固定
        playerPos.z + dist(gen)
    };

    // 新しい敵を生成
    auto newEnemy = std::make_unique<Enemy>();
    newEnemy->Initialize(camera_);
    newEnemy->SetPosition(spawnPos);
    newEnemy->SetPlayer(player_);

    // リストに追加
    enemies_.push_back(std::move(newEnemy));
}