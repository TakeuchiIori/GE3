#pragma once
#include <string>

class ObjectTime {
public:
  ObjectTime();

  /// <summary>
  /// 時間の更新
  /// </summary>
  /// <param name="deltaTime"></param>
  void Update(float deltaTime);

  /// <summary>
  /// オブジェクトの時間を取得
  /// </summary>
  /// <returns></returns>
  float GetTime() const { return time_; }

private:
  float time_; // オブジェクト固有の時間
};
