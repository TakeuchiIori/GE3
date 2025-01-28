#include "Camera.h"
// C++
#include <cmath>
// Engine
#include "WinApp./WinApp.h"

// Math
#include "MathFunc.h"
#include "Matrix4x4.h"
#include "Systems/Input/Input.h"

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

}

void Camera::UpdateMatrix()
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

void Camera::ShowImGui()
{
#ifdef _DEBUG
    ImGui::Begin("Camera ");
    ImGui::DragFloat3("Camera Position", &transform_.translate.x, 0.01f);
    ImGui::DragFloat3("Camera Rotate", &transform_.rotate.x, 0.01f);
    ImGui::End();
#endif
}

void Camera::DefaultCamera()
{
    // カメラの位置と回転をリセット（原点に設定）
    transform_.translate = Vector3(0.0f, 5.0f, -100.0f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);

    // ワールド行列、ビュー行列の更新
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    viewMatrix_ = Inverse(worldMatrix_);
    viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);

}
