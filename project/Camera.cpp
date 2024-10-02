#include "Camera.h"
#include "MathFunc.h"
void Camera::Update()
{
	// transformからアフィン変換行列を計算
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// worldMatrixの逆行列
	viewMatrix_ = Inverse(worldMatrix_);
}
