#include "MathFunc.h"
// 関数の実装
float Dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float MagnitudeSquared(const Vector3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vector3 Subtract(const Vector3& a, const Vector3& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3 Multiply(const Vector3& v, float scalar) {
    return { v.x * scalar, v.y * scalar, v.z * scalar };
}

float Magnitude(const float& v) {
    return std::sqrt(v * v);
}

float Magnitude(const Vector3& v) {
    return std::sqrt(MagnitudeSquared(v));
}

float Magnitude(const Vector4& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float Normalize(const float& v) {
    return v / std::fabs(v); // 修正：v/v は問題があるため、fabs を使います
}

Vector3 Normalize(const Vector3& v) {
    float mag = Magnitude(v);
    return { v.x / mag, v.y / mag, v.z / mag };
}

Vector4 Normalize(const Vector4& v) {
    float mag = Magnitude(v);
    return { v.x / mag, v.y / mag, v.z / mag , v.w / mag };
}

float Distance(const Vector3& a, const Vector3& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
Matrix4x4 ScaleMatrixFromVector3(const Vector3& scale)
{
    Matrix4x4 scaleMatrix = {};
    scaleMatrix.m[0][0] = scale.x;  // X方向のスケール
    scaleMatrix.m[1][1] = scale.y;  // Y方向のスケール
    scaleMatrix.m[2][2] = scale.z;  // Z方向のスケール
    scaleMatrix.m[3][3] = 1.0f;     // W成分は1.0

    return scaleMatrix;
}

Matrix4x4 TranslationMatrixFromVector3(const Vector3& translate)
{
    Matrix4x4 translationMatrix = {};
    translationMatrix.m[0][0] = 1.0f;  // 単位行列の成分
    translationMatrix.m[1][1] = 1.0f;  // 単位行列の成分
    translationMatrix.m[2][2] = 1.0f;  // 単位行列の成分
    translationMatrix.m[3][3] = 1.0f;  // 単位行列の成分

    translationMatrix.m[3][0] = translate.x;  // X方向の移動
    translationMatrix.m[3][1] = translate.y;  // Y方向の移動
    translationMatrix.m[3][2] = translate.z;  // Z方向の移動

    return translationMatrix;
}
