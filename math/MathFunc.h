#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>
// ベクトルの内積を計算する関数
float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
// ベクトルの大きさの2乗を計算する関数
float MagnitudeSquared(const Vector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
// ベクトルの引き算を行う関数
Vector3 Subtract(const Vector3& a, const Vector3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
// ベクトルの掛け算を行う関数
Vector3 Multiply(const Vector3& v, float scalar) { return { v.x * scalar, v.y * scalar, v.z * scalar }; }
// ベクトルの大きさを計算する関数

// float
float Magnitude(const float& v) { return std::sqrt(v * v); }
// Vector3 
float Magnitude(const Vector3& v) { return std::sqrt(MagnitudeSquared(v)); }
// Vector4
 float Magnitude(const Vector4 & v) { return std::sqrt(v.x * v.x+ v.y * v.y + v.z * v.z + v.w * v.w ); }

// ベクトルの正規化を行う関数
 float Normalize(const float& v) {
	 float mag = Magnitude(v);
	 return { v / v };
 }
Vector3 Normalize(const Vector3& v) {
	float mag = Magnitude(v);
	return { v.x / mag, v.y / mag, v.z / mag };
}
// Vector4
Vector4 Normalize(const Vector4& v) {
	float mag = Magnitude(v);
	return { v.x / mag, v.y / mag, v.z / mag ,v.w / mag};
}

// ベクトル間の距離を計算する関数
float Distance(const Vector3& a, const Vector3& b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float dz = b.z - a.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}
