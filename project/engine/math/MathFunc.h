#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "vector"
#include <stdexcept>

struct Sphere {
	Vector3 center; // !< 中心点
	float radius;   // !< 半径
};
struct Plane {
	Vector3 normal; // !<法線
	float distance; // !<距離
};
struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct Triangle {
	Vector3 vertex[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
};
struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

// 関数の宣言
float Dot(const Vector3& a, const Vector3& b);
float MagnitudeSquared(const Vector3& v);
Vector3 Multiply(const Vector3& v, float scalar);
float Magnitude(const float& v);
float Magnitude(const Vector3& v);
float Magnitude(const Vector4& v);
float Normalize(const float& v);
Vector3 Normalize(const Vector3& v);
Vector4 Normalize(const Vector4& v);
float Distance(const Vector3& a, const Vector3& b);
Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t);
std::vector<Vector3> GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t segmentCount);

// Vector3 : 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// Vector3 : 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
// 正規化
float LengthSquared(const Vector3&);
float Length(const Vector3& v);
Matrix4x4 ScaleMatrixFromVector3(const Vector3& scale);
Matrix4x4 TranslationMatrixFromVector3(const Vector3& translate);
bool IsCollision(const AABB& aabb, const Vector3& point);
bool IsCollision(const AABB& aabb, const Sphere& sphere);