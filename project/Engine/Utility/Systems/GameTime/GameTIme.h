#pragma once
#include "ObjectTime.h"
#include <string>
#include <unordered_map>

class GameTime {
public:
  GameTime() : globalTime_(0.0f) {}
  ~GameTime() = default;

  /// <summary>
  /// ゲーム全体の時間を更新
  /// </summary>
  /// <param name="deltaTime"></param>
  void GameUpdate(float deltaTime);

  /// <summary>
  /// ゲーム全体の時間を取得
  /// </summary>
  /// <returns></returns>
  float GetGlobalTime() const { return globalTime_; }

  /// <summary>
  /// オブジェクトのIDを文字列で設定して、そのオブジェクトの時間を管理
  /// </summary>
  /// <param name="id"></param>
  void RegisterObject(const std::string &id);

  /// <summary>
  /// 指定したIDのオブジェクトの時間を取得
  /// </summary>
  /// <param name="id"></param>
  /// <returns></returns>
  float GetObjectTime(const std::string &id) const;

  /// <summary>
  /// 指定したIDのオブジェクトの時間を更新
  /// </summary>
  /// <param name="id"></param>
  /// <param name="deltaTime"></param>
  void UpdateObjectTime(const std::string &id, float deltaTime);

private:
  float globalTime_; 									    // ゲーム全体の時間
  std::unordered_map<std::string, ObjectTime> objectTimes_; // オブジェクトIDごとの時間
};
