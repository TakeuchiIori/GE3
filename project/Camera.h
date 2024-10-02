#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
class Camera
{
public: // メンバ関数


	/// <summary>
	/// 更新
	/// </summary>
	void Update();




private: // メンバ変数
	Transform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
};

