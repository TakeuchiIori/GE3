#pragma once
#include <cmath>
struct Vector3 final {
    float x;
    float y;
    float z;

    // ベクトルの加算演算子オーバーロード
    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    // ベクトルの引き算演算子オーバーロード
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    friend Vector3 operator-(float scalar, const Vector3& vec) {
        return { vec.x - scalar, vec.y - scalar, vec.z - scalar };
    }

    // ベクトルの加算代入演算子オーバーロード
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // スカラーとの乗算の演算子オーバーロード
    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    // スカラーを掛け算する演算子の反転（float * Vector3 の場合も対応）
    friend Vector3 operator*(float scalar, const Vector3& vec) {
        return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
    }

    // スカラーとの割り算の演算子オーバーロード
    Vector3 operator/(float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }

    // デフォルトコンストラクタ
    Vector3() : x(0), y(0), z(0) {}

    // 引数付きコンストラクタ
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // ベクトルの外積を計算する関数
    Vector3 Cross(const Vector3& a, const Vector3& b) {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    // ベクトルの長さを計算する関数
    float Length(const Vector3& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
