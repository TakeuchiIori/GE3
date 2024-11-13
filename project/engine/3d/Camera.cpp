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

void Camera::ResetToOrigin()
{
    // カメラの位置と回転をリセット（原点に設定）
    transform_.translate = Vector3(0.0f, 5.0f, -60.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);

    // ワールド行列、ビュー行列の更新
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
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


void Camera::SetFPSCamera(const Vector3& position, const Vector3& rotation)
{
    // カメラ位置をプレイヤーの位置に合わせ、目の高さに調整
    transform_.translate = position;
    transform_.translate.y += 1.5f;
    transform_.translate.z += 1.2f; // 目の高さを設定

    // 回転はプレイヤーの回転のみを反映し、位置の移動とは独立させる
    transform_.rotate = rotation;

    // アフィン変換行列を再計算
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
    //viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
  
}