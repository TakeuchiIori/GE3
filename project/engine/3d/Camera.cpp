#include "Camera.h"
#include "MathFunc.h"
#include "WinApp.h"
#include <cmath>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

Camera::Camera()
    : transform_({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })
    , fovY_(0.45f)
    , aspectRatio_(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
    , nearClip_(0.1f)
    , farClip_(100.0f)
    , worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
    , viewMatrix_(Inverse(worldMatrix_))
    , projectionMatrix_(MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_))
    , viewProjectionMatrix_(Multiply(viewMatrix_, projectionMatrix_))
{}

void Camera::Update()
{
	// transformからアフィン変換行列を計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    // worldMatrixの逆行列
    viewMatrix_ = Inverse(worldMatrix_);
    // プロジェクション行列の更新
    projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
    // 合成行列
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::FollowCamera(Vector3& target)
{	// カメラの位置を対象の後方に設定
#ifdef _DEBUG
    ImGui::Begin("Camera ");
    ImGui::DragFloat3("Follow position", &followCameraOffsetPosition_.x, 0.01f);
    ImGui::DragFloat3("Follow Rotate", &followCameraOffsetPosition_.x, 0.01f);
    ImGui::End();
#endif
    transform_.rotate = followCameraOffsetRotare_;
    transform_.translate = target + followCameraOffsetPosition_; 
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
}

void Camera::SetTopDownCamera(const Vector3& position)
{
    transform_.translate = position;
    transform_.rotate = Vector3(1.57f, 0.0f, 0.0f); // 真上から見下ろすように設定
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
}

/// <summary>
/// カメラの向きを右スティックで操作する関数
/// </summary>
/// <param name="rightStickX">右スティックのX軸の入力値</param>
/// <param name="rightStickY">右スティックのY軸の入力値</param>
/// <param name="sensitivity">回転の感度</param>
void Camera::ControlCameraWithRightStick(float rightStickX, float rightStickY, float sensitivity = 0.1f)
{
    // 入力値に基づき回転量を計算
    transform_.rotate.y += rightStickX * sensitivity; // 左右回転
    transform_.rotate.x -= rightStickY * sensitivity; // 上下回転

    // 回転制限 (上下回転を制限し、カメラが真上や真下を向かないようにする)
    if (transform_.rotate.x > 1.57f) transform_.rotate.x = 1.57f;   // 上方向の限界
    if (transform_.rotate.x < -1.57f) transform_.rotate.x = -1.57f; // 下方向の限界

    // 変換行列を更新
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::SetFPSCamera(const Vector3& position, const Vector3& rotation)
{
    // カメラ位置をプレイヤー位置に設定
    transform_.translate = position;

    transform_.translate.z -= 10.5f;
    // カメラの回転をプレイヤーの回転と同じに設定
    transform_.rotate = rotation;

    // 変換行列を更新
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}



