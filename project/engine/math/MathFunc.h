#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

// 関数の宣言
float Dot(const Vector3& a, const Vector3& b);
float MagnitudeSquared(const Vector3& v);
Vector3 Subtract(const Vector3& a, const Vector3& b);
Vector3 Multiply(const Vector3& v, float scalar);
float Magnitude(const float& v);
float Magnitude(const Vector3& v);
float Magnitude(const Vector4& v);
float Normalize(const float& v);
Vector3 Normalize(const Vector3& v);
Vector4 Normalize(const Vector4& v);
float Distance(const Vector3& a, const Vector3& b);
