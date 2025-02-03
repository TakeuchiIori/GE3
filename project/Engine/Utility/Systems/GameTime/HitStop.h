#pragma once


class HitStop {
public:
    HitStop() : stopDuration_(0.0f), isStopping_(false), elapsedTime_(0.0f) {}

    // ヒットストップを開始する
    void Start(float duration) {
        stopDuration_ = duration;
        isStopping_ = true;
        elapsedTime_ = 0.0f;
    }

    // ヒットストップの更新
    void Update(float deltaTime) {
        if (!isStopping_) return;

        elapsedTime_ += deltaTime;
        if (elapsedTime_ >= stopDuration_) {
            isStopping_ = false;
        }
    }

    // ヒットストップ中かどうか
    bool IsActive() const {
        return isStopping_;
    }

private:
    float stopDuration_;   // ヒットストップの持続時間
    bool isStopping_;      // ヒットストップが有効かどうか
    float elapsedTime_;    // 経過時間
};

