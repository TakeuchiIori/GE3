#pragma once
#include <cmath>

struct Vector3 final {
    float x;
    float y;
    float z;

    // コンストラクタ
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // 加算演算子 +
    Vector3 operator+(const Vector3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    // 加算代入演算子 +=
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // 減算演算子 -
    Vector3 operator-(const Vector3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    // 減算代入演算子 -=
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    // スカラーとの乗算演算子 *
    Vector3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    // スカラー乗算代入演算子 *=
    Vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // スカラーを掛け算する演算子の反転（float * Vector3 の場合も対応）
    friend Vector3 operator*(float scalar, const Vector3& vec) {
        return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
    }



    // スカラーとの割り算演算子 /
    Vector3 operator/(float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }

    // スカラーを減算する演算子の反転（float - Vector3 の場合も対応）
    friend Vector3 operator-(float scalar, const Vector3& vec) {
        return { scalar - vec.x, scalar - vec.y, scalar - vec.z };
    }

    // ベクトルの外積を計算する関数
    static Vector3 Cross(const Vector3& a, const Vector3& b) {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    // ベクトルの長さを計算する関数
    static float Length(const Vector3& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    // ベクトルを正規化する関数
    static Vector3 Normalize(const Vector3& v) {
        float length = Length(v);
        if (length == 0.0f) {
            return Vector3(0.0f, 0.0f, 0.0f); // ゼロベクトルはそのまま返す
        }
        return v / length;
    }

    // ベクトルの内積を計算する関数
    static float Dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // ゼロベクトルかを確認する関数
    bool IsZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }
};

/// <summary>
/// Vector3型同士を等値比較する演算子
/// </summary>
inline bool operator==(const Vector3& lhs, const Vector3& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

/// <summary>
/// Vector3型同士を非等値比較する演算子
/// </summary>
inline bool operator!=(const Vector3& lhs, const Vector3& rhs) {
    return !(lhs == rhs);
}