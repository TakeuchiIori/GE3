#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Quaternion
{
	float x, y, z, w;
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

	Quaternion operator+(const Quaternion& q) const;
	Quaternion operator-(const Quaternion& q) const;
	Quaternion& operator+=(const Quaternion& q);
	Quaternion& operator-=(const Quaternion& q);
	// スカラー倍演算子
	Quaternion operator*(float scalar) const;
	Quaternion& operator*=(float scalar);

};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};


// Quaternion回転を含むアフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);

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

// 複数の回転を合成
Quaternion CombineRotations(const Quaternion& q1, const Quaternion& q2);

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& angles);

Quaternion CombineRotations(const Quaternion& lhs, const Quaternion& rhs);

// クォータニオンを使ってベクトルを回転させる関数
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

// クォータニオンから回転行列を作成する関数
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

// 2つのクォータニオンの内積を計算する関数
float Dot(const Quaternion& q0, const Quaternion& q1);

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

// 2つのクォータニオン間で球面線形補間（Slerp）を行う関数
Quaternion Slerp(Quaternion q0, Quaternion q1, float t);

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
