#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "vector"
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <algorithm>

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
struct Quaternion
{
    float w, x, y, z;
	// 単位クォータニオンを簡単に返すための静的メンバ
	static Quaternion Identity() {
		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	// クォータニオンの掛け算演算子オーバーロード
	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		);
	}
};

// ベクトルの内積を計算する関数
float Dot(const Vector3& a, const Vector3& b);

// ベクトルの大きさの二乗を計算する関数
float MagnitudeSquared(const Vector3& v);

// ベクトルにスカラーを掛け算する関数
Vector3 Multiply(const Vector3& v, float scalar);

// スカラー値の絶対値を計算する関数
float Magnitude(const float& v);

// Vector3の大きさを計算する関数
float Magnitude(const Vector3& v);

// Vector4の大きさを計算する関数
float Magnitude(const Vector4& v);

// スカラー値を正規化する関数
float Normalize(const float& v);

// Vector3を正規化する関数
Vector3 Normalize(const Vector3& v);

// Vector4を正規化する関数
Vector4 Normalize(const Vector4& v);

// 2つのVector3間の距離を計算する関数
float Distance(const Vector3& a, const Vector3& b);

// Catmull-Romスプライン補間を用いて曲線上の点を計算する関数
Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t);

// Catmull-Romスプラインのポイントを生成する関数
std::vector<Vector3> GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t segmentCount);

// Vector3同士の加算を行う関数
Vector3 Add(const Vector3& v1, const Vector3& v2);

// Vector3同士の減算を行う関数
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

// ベクトルのクロス積を計算する関数
Vector3 Cross(const Vector3& v1, const Vector3& v2);

Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

// Vector3の長さの二乗を計算する関数
float LengthSquared(const Vector3& v);

// Vector3の長さを計算する関数
float Length(const Vector3& v);

// Vector3からスケール行列を作成する関数
Matrix4x4 ScaleMatrixFromVector3(const Vector3& scale);

// Vector3から平行移動行列を作成する関数
Matrix4x4 TranslationMatrixFromVector3(const Vector3& translate);

// AABBと点の衝突判定を行う関数
bool IsCollision(const AABB& aabb, const Vector3& point);

// AABBと球の衝突判定を行う関数
bool IsCollision(const AABB& aabb, const Sphere& sphere);

// ぐちずえんじんご提供
Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

// ぐちずえんじんご提供
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

// 単位クォータニオンを返す関数
Quaternion IdentityQuaternion();

// 2つのクォータニオンの積を計算する関数
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// クォータニオンの共役を返す関数
Quaternion Conjugate(const Quaternion& quaternion);

// クォータニオンのノルム（大きさ）を計算する関数
float Norm(const Quaternion& quaternion);

// クォータニオンを正規化する関数
Quaternion Normalize(const Quaternion& quaternion);

// クォータニオンの逆を計算する関数
Quaternion Inverse(const Quaternion& quaternion);

// 指定した軸と角度で回転を表すクォータニオンを作成する関数
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

// クォータニオンを使ってベクトルを回転させる関数
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

// クォータニオンから回転行列を作成する関数
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

// 2つのクォータニオンの内積を計算する関数
float Dot(const Quaternion& q0, const Quaternion& q1);

// 2つのクォータニオン間で球面線形補間（Slerp）を行う関数
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

// クォータニオンからオイラー角を作成する関数
Vector3 QuaternionToEuler(const Quaternion& q);

// 2つの方向ベクトルを揃えるクォータニオンを計算
Quaternion MakeAlignQuaternion(const Vector3& from, const Vector3& to);

Vector3 SetFromTo(const Vector3& from, const Vector3& to);

Quaternion SetFromToQuaternion(const Vector3& from, const Vector3& to);

Vector3 RotateVectorByQuaternion(const Vector3& vec, const Quaternion& quat);

Quaternion EulerToQuaternion(const Vector3& euler);

Quaternion MatrixToQuaternion(const Matrix4x4& mat);

Quaternion LookAtQuaternion(const Vector3& from, const Vector3& to, const Vector3& up);

// Quaternionから前方向ベクトルを取得する関数
Vector3 QuaternionToForward(const Quaternion& quat);
