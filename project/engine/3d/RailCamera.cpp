#include "RailCamera.h"

RailCamera::RailCamera() : splinePoints_(nullptr), currentPosition_(0.0f) {
    worldTransform_.Initialize();
}

void RailCamera::SetSplinePath(const std::vector<Vector3>& splinePoints) {
    splinePoints_ = &splinePoints;
    currentPosition_ = 0.0f;
}

void RailCamera::SetParent(const WorldTransform* parent) {
    worldTransform_.parent_ = parent;
}

void RailCamera::Update() {
    if (!splinePoints_ || splinePoints_->empty()) return;

    // インデックスと補間用の小数部分を分離
    size_t index = static_cast<size_t>(currentPosition_);
    float t = currentPosition_ - index;

    // 次の位置のインデックスを計算
    size_t nextIndex = (index + 1) % splinePoints_->size();

    // 現在の位置と次の位置の間を補間
    Vector3 currentPoint = (*splinePoints_)[index];
    Vector3 nextPoint = (*splinePoints_)[nextIndex];
    worldTransform_.translation_ = Lerp(currentPoint, nextPoint, t);

    // 方向ベクトルを求め、回転設定
    Vector3 direction = Normalize(nextPoint - currentPoint);
    Quaternion rotationQuat = SetFromToQuaternion(Vector3(0.0f, 0.0f, 1.0f), direction);
    worldTransform_.rotation_ = QuaternionToEuler(rotationQuat);

    // ワールドトランスフォームを更新
    worldTransform_.UpdateMatrix();

    // 位置を進める
    currentPosition_ += speed_;
    if (currentPosition_ >= splinePoints_->size()) {
        currentPosition_ -= splinePoints_->size();
    }
}

