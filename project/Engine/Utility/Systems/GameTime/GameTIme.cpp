#include "GameTIme.h"
#include <stdexcept>

GameTime* GameTime::GetInstance()
{
    static GameTime instance;
    return &instance;
}

void GameTime::Initialize()
{
    globalTime_ = 0.0f;
    //objectTimes_.clear();
}

/// ゲーム全体の時間を更新
void GameTime::GameUpdate(float deltaTime) {
  globalTime_ += deltaTime;

  for (auto &objTime : objectTimes_) {
    objTime.second.Update(deltaTime);   // オブジェクトごとの時間を更新
  }
}

/// オブジェクトのIDを文字列で設定して、そのオブジェクトの時間を管理
void GameTime::RegisterObject(const std::string &id) {
  objectTimes_[id] = ObjectTime();      // 新しいオブジェクト時間を登録
}

/// 指定したIDのオブジェクトの時間を取得
float GameTime::GetObjectTime(const std::string &id) const {
  auto it = objectTimes_.find(id);
  if (it != objectTimes_.end()) {
    return it->second.GetTime();
  }
  throw std::runtime_error("ObjectTime not found" + id);
}

/// 指定したIDのオブジェクトの時間を更新
void GameTime::UpdateObjectTime(const std::string &id, float deltaTime) {
  auto it = objectTimes_.find(id);
  if (it != objectTimes_.end()) {
    it->second.Update(deltaTime);
  }
}
