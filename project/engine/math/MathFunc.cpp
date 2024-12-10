#include "MathFunc.h"


float Dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float MagnitudeSquared(const Vector3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}


// Vector3 : 加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
    Vector3 result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}
// Vector3 : 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}
Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
{
    return a * (1.0f - t) + b * t;
}

float Lerp(float a, float b, float t)
{
    return (1.0f - t) * a + t * b;
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
Vector3 Normalize(Vector3& vec) {
    // 実装例：ベクトルを正規化する関数
    float length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (length > 0) {
        return Vector3{ vec.x / length, vec.y / length, vec.z / length };
    }
    return Vector3{ 0, 0, 0 };
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

Vector3 CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t) {
    // コントロールポイントが4つでないときはエラーを表示する。
    if (controlPoints.size() != 4) {
        throw std::invalid_argument("Catmull-Rom Splineには4つのコントロールポイントが必要です。");
    }

    // t の2乗と3乗を計算
    float t2 = t * t;
    float t3 = t2 * t;

    // 各コントロールポイントを変数に代入
    const Vector3& p0 = controlPoints[0];
    const Vector3& p1 = controlPoints[1];
    const Vector3& p2 = controlPoints[2];
    const Vector3& p3 = controlPoints[3];

    // X, Y, Z 軸成分の Catmull-Rom 補間計算
    float x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    float y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
    float z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);

    // 計算された位置をVector3で返す
    return Vector3(x, y, z);
}

std::vector<Vector3> GenerateCatmullRomSplinePoints(const std::vector<Vector3>& controlPoints, size_t segmentCount) {
    std::vector<Vector3> pointsDrawing;

    // コントロールポイント数の確認
    if (controlPoints.size() < 4) {
        throw std::invalid_argument("Catmull-Rom Splineには少なくとも4つのコントロールポイントが必要です。");
    }

    // 各4つのコントロールポイントについて補間を行う
    for (size_t i = 0; i < controlPoints.size() - 3; ++i) {
        std::vector<Vector3> segmentControlPoints = {
            controlPoints[i],
            controlPoints[i + 1],
            controlPoints[i + 2],
            controlPoints[i + 3]
        };

        // セグメントを分割して頂点を生成
        for (size_t j = 0; j <= segmentCount; ++j) {
            float t = static_cast<float>(j) / static_cast<float>(segmentCount);
            Vector3 pos = CatmullRomSpline(segmentControlPoints, t);
            pointsDrawing.push_back(pos);
        }
    }

    return pointsDrawing;
}


float LengthSquared(const Vector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

float Length(const Vector3& v) { return std::sqrt(LengthSquared(v)); }

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
bool IsCollision(const AABB& aabb, const Vector3& point) {
    if (point.x < aabb.min.x || point.x > aabb.max.x) return false;
    if (point.y < aabb.min.y || point.y > aabb.max.y) return false;
    if (point.z < aabb.min.z || point.z > aabb.max.z) return false;
    return true;
}
bool IsCollision(const AABB& aabb, const Sphere& sphere) {
    Vector3 closestPointInAABB = sphere.center;

    if (sphere.center.x < aabb.min.x) closestPointInAABB.x = aabb.min.x;
    if (sphere.center.y < aabb.min.y) closestPointInAABB.y = aabb.min.y;
    if (sphere.center.z < aabb.min.z) closestPointInAABB.z = aabb.min.z;

    if (sphere.center.x > aabb.max.x) closestPointInAABB.x = aabb.max.x;
    if (sphere.center.y > aabb.max.y) closestPointInAABB.y = aabb.max.y;
    if (sphere.center.z > aabb.max.z) closestPointInAABB.z = aabb.max.z;

    Vector3 difference = closestPointInAABB - sphere.center;
    float distanceSquared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;

    return distanceSquared < (sphere.radius * sphere.radius);
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    const float s = 0.5f;

    float t2 = t * t;
    float t3 = t2 * t;

    Vector3 e3 = -1 * p0 + 3 * p1 - 3 * p2 + p3;
    Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
    Vector3 e1 = -1 * p0 + p2;
    Vector3 e0 = 2 * p1;


    return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
};


Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
    assert(points.size() >= 4 && "制御点は4点以上必要です");

    // 区間数は制御点の数-1
    size_t division = points.size() - 1;
    // 1区間の長さ
    float areaWidth = 1.0f / division;

    // 区間内の始点を0.0f, 終点を1.0fとした時の現在位置
    float t_2 = std::fmod(t, areaWidth) * division;
    // 加減(0.0f)と上限(1.0f)の範囲に収める
    t_2 = std::clamp(t_2, 0.0f, 1.0f);

    // 区間番号を計算し、範囲内に収める
    size_t index = static_cast<size_t>(t / areaWidth);
    // 上限を超えないように抑える
    index = std::min(index, points.size() - 2);

    // 4点分のインデックス
    size_t index0 = index - 1;
    size_t index1 = index;
    size_t index2 = index + 1;
    size_t index3 = index + 2;

    // 最初の区間のp0はp1を重複使用する
    if (index == 0) {
        index0 = index1;
    }
    // 最後の区間のp3はp2を重複使用する
    if (index3 >= points.size()) {
        index3 = index2;
    }

    // 4点の座標
    const Vector3& p0 = points[index0];
    const Vector3& p1 = points[index1];
    const Vector3& p2 = points[index2];
    const Vector3& p3 = points[index3];

    return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}



Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
{
    // スケーリング行列を生成
    Matrix4x4 scalingMatrix = MakeScaleMatrix(scale);

    // 回転行列を生成
    Matrix4x4 rotationMatrix = MakeRotateMatrix(rotate);

    // 平行移動行列を生成
    Matrix4x4 translationMatrix = MakeTranslateMatrix(translate);

    // 行列を結合してアフィン変換行列を生成
    return translationMatrix * rotationMatrix * scalingMatrix;
}



Quaternion IdentityQuaternion()
{
    return { 0, 0, 0, 1 };
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
    return {
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
        lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    };
}

Quaternion Conjugate(const Quaternion& quaternion) {
    return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

float Norm(const Quaternion& quaternion) {
    return std::sqrt(quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion) {
    float norm = Norm(quaternion);
    if (norm == 0.0f) {
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    return { quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

Quaternion Inverse(const Quaternion& quaternion) {
    Quaternion conjugate = Conjugate(quaternion);
    float norm = Norm(quaternion);
    float normSq = norm * norm;
    if (normSq == 0.0f) {
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    return { conjugate.x / normSq, conjugate.y / normSq, conjugate.z / normSq, conjugate.w / normSq };
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
    Vector3 normAxis = Normalize(axis);
    float sinHalfAngle = std::sin(angle / 2.0f);
    float cosHalfAngle = std::cos(angle / 2.0f);
    return { normAxis.x * sinHalfAngle, normAxis.y * sinHalfAngle, normAxis.z * sinHalfAngle, cosHalfAngle };
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
    Quaternion qVector = { vector.x, vector.y, vector.z, 0.0f };
    Quaternion qConjugate = { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
    Quaternion qResult = Multiply(Multiply(quaternion, qVector), qConjugate);
    return { qResult.x, qResult.y, qResult.z };
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
    Matrix4x4 matrix = {};

    float xx = quaternion.x * quaternion.x;
    float yy = quaternion.y * quaternion.y;
    float zz = quaternion.z * quaternion.z;
    float xy = quaternion.x * quaternion.y;
    float xz = quaternion.x * quaternion.z;
    float yz = quaternion.y * quaternion.z;
    float wx = quaternion.w * quaternion.x;
    float wy = quaternion.w * quaternion.y;
    float wz = quaternion.w * quaternion.z;

    matrix.m[0][0] = 1.0f - 2.0f * (yy + zz);
    matrix.m[0][1] = 2.0f * (xy - wz);
    matrix.m[0][2] = 2.0f * (xz + wy);
    matrix.m[0][3] = 0.0f;

    matrix.m[1][0] = 2.0f * (xy + wz);
    matrix.m[1][1] = 1.0f - 2.0f * (xx + zz);
    matrix.m[1][2] = 2.0f * (yz - wx);
    matrix.m[1][3] = 0.0f;

    matrix.m[2][0] = 2.0f * (xz - wy);
    matrix.m[2][1] = 2.0f * (yz + wx);
    matrix.m[2][2] = 1.0f - 2.0f * (xx + yy);
    matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.0f;

    matrix = TransPose(matrix);

    return matrix;
}

float Dot(const Quaternion& q0, const Quaternion& q1) {
    return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
    // t が範囲外の場合はクランプする
    float tt = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);

    // 線形補間 (1 - t)*q1 + t*q2
    Quaternion result;
    result.x = (1.0f - tt) * q1.x + tt * q2.x;
    result.y = (1.0f - tt) * q1.y + tt * q2.y;
    result.z = (1.0f - tt) * q1.z + tt * q2.z;
    result.w = (1.0f - tt) * q1.w + tt * q2.w;

    // 結果を正規化
    float len = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
    if (len > 0.0f)
    {
        float invLen = 1.0f / len;
        result.x *= invLen;
        result.y *= invLen;
        result.z *= invLen;
        result.w *= invLen;
    }

    return result;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
    // クォータニオンの内積を計算
    float dot = Dot(q0, q1);

    // クォータニオンが反対向きの場合、内積が負になるので符号を反転
    const float THRESHOLD = 0.9995f;
    if (dot < 0.0f) {
        dot = -dot;
        Quaternion negQ1 = { -q1.x, -q1.y, -q1.z, -q1.w };
        return Slerp(q0, negQ1, t);
    }

    // 内積が閾値以上の場合、線形補間を使用
    if (dot > THRESHOLD) {
        Quaternion result = {
            q0.x + t * (q1.x - q0.x),
            q0.y + t * (q1.y - q0.y),
            q0.z + t * (q1.z - q0.z),
            q0.w + t * (q1.w - q0.w)
        };
        // 正規化
        float norm = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
        return { result.x / norm, result.y / norm, result.z / norm, result.w / norm };
    }

    // 角度を計算
    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);

    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return {
        s0 * q0.x + s1 * q1.x,
        s0 * q0.y + s1 * q1.y,
        s0 * q0.z + s1 * q1.z,
        s0 * q0.w + s1 * q1.w
    };
}

// クォータニオンからオイラー角を作成する関数
Vector3 QuaternionToEuler(const Quaternion& q)
{
    Vector3 euler;

    // Roll (X軸回転)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (Y軸回転)
    float sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        euler.y = static_cast<float>(std::copysign(std::numbers::pi / 2, sinp)); // Gimbal lock
    else
        euler.y = std::asin(sinp);

    // Yaw (Z軸回転)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z = std::atan2(siny_cosp, cosy_cosp);

    return euler;
}

Quaternion MakeAlignQuaternion(const Vector3& from, const Vector3& to) {
    Vector3 cross = Cross(from, to);
    float dot = Dot(from, to);

    if (dot >= 1.0f) {
        return Quaternion(0, 0, 0, 1); // 同じ方向の場合、回転なし
    }
    else if (dot <= -1.0f) {
        // 反対方向の場合、回転軸を選択する必要がある
        Vector3 axis = (std::abs(from.x) < std::abs(from.y)) ? Vector3(1, 0, 0) : Vector3(0, 1, 0);
        axis = Normalize(Cross(from, axis));
        return Quaternion(axis.x, axis.y, axis.z, 0); // 180度回転
    }

    float s = std::sqrt((1 + dot) * 2);
    float invs = 1 / s;

    return Quaternion(cross.x * invs, cross.y * invs, cross.z * invs, s * 0.5f);
}

/// <summary>
/// 2つのベクトルの間の回転を計算する関数
/// </summary>
Vector3 SetFromTo(const Vector3& from, const Vector3& to) {
    // ベクトルを正規化
    Vector3 normalizedFrom = Normalize(from);
    Vector3 normalizedTo = Normalize(to);

    // 内積を計算して、ベクトルの関係性を判断
    float dot = Dot(normalizedFrom, normalizedTo);
    Vector3 rotationAxis;
    float rotationAngle;

    // ベクトルが同じ方向の場合、回転は不要
    if (dot > 0.9999f) {
        return Vector3(0.0f, 0.0f, 0.0f);
    }
    // ベクトルが逆方向の場合、任意の垂直軸を使用して180度回転
    else if (dot < -0.9999f) {
        rotationAxis = Cross(Vector3(1.0f, 0.0f, 0.0f), normalizedFrom);
        if (Length(rotationAxis) < 0.0001f) {
            rotationAxis = Cross(Vector3(0.0f, 1.0f, 0.0f), normalizedFrom);
        }
        rotationAxis = Normalize(rotationAxis);
        rotationAngle = static_cast<float>(std::numbers::pi);
    }
    // それ以外の場合、通常の回転を計算
    else {
        rotationAxis = Normalize(Cross(normalizedFrom, normalizedTo));
        rotationAngle = acos(dot);
    }

    // オイラー角に変換（クォータニオン不要の場合）
    return rotationAxis * rotationAngle;
}

/// <summary>
/// 2つのベクトル間の回転を表すクォータニオンを生成する関数
/// </summary>
Quaternion SetFromToQuaternion(const Vector3& from, const Vector3& to) {

    Vector3 f = Normalize(from);
    Vector3 t = Normalize(to);

    Vector3 cross = Cross(f, t);

    float dot = Dot(f, t);
    float w = sqrt((1 + dot) * 0.5f);
    float s = 0.5f / w;


    float x = cross.x * s;
    float y = cross.y * s;
    float z = cross.z * s;

    return Quaternion{ x, y, z, w };
}

Vector3 RotateVectorByQuaternion(const Vector3& vec, const Quaternion& quat) {
    Quaternion qVec(0, vec.x, vec.y, vec.z);
    Quaternion result = quat * qVec * Conjugate(quat);
    return Vector3(result.x, result.y, result.z);
}

// オイラー角をクォータニオンに変換する関数
Quaternion EulerToQuaternion(const Vector3& euler)
{
    // オイラー角をラジアンに変換
    float pitch = euler.x;
    float yaw = euler.y;
    float roll = euler.z;

    // 半角を計算
    float cy = cos(yaw * 0.5f);
    float sy = sin(yaw * 0.5f);
    float cp = cos(pitch * 0.5f);
    float sp = sin(pitch * 0.5f);
    float cr = cos(roll * 0.5f);
    float sr = sin(roll * 0.5f);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    return q;
}
Quaternion MatrixToQuaternion(const Matrix4x4& mat) {
    Quaternion q;
    float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];

    if (trace > 0.0f) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        q.w = 0.25f / s;
        q.x = (mat.m[2][1] - mat.m[1][2]) * s;
        q.y = (mat.m[0][2] - mat.m[2][0]) * s;
        q.z = (mat.m[1][0] - mat.m[0][1]) * s;
    }
    else {
        if (mat.m[0][0] > mat.m[1][1] && mat.m[0][0] > mat.m[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat.m[0][0] - mat.m[1][1] - mat.m[2][2]);
            q.w = (mat.m[2][1] - mat.m[1][2]) / s;
            q.x = 0.25f * s;
            q.y = (mat.m[0][1] + mat.m[1][0]) / s;
            q.z = (mat.m[0][2] + mat.m[2][0]) / s;
        }
        else if (mat.m[1][1] > mat.m[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat.m[1][1] - mat.m[0][0] - mat.m[2][2]);
            q.w = (mat.m[0][2] - mat.m[2][0]) / s;
            q.x = (mat.m[0][1] + mat.m[1][0]) / s;
            q.y = 0.25f * s;
            q.z = (mat.m[1][2] + mat.m[2][1]) / s;
        }
        else {
            float s = 2.0f * sqrtf(1.0f + mat.m[2][2] - mat.m[0][0] - mat.m[1][1]);
            q.w = (mat.m[1][0] - mat.m[0][1]) / s;
            q.x = (mat.m[0][2] + mat.m[2][0]) / s;
            q.y = (mat.m[1][2] + mat.m[2][1]) / s;
            q.z = 0.25f * s;
        }
    }
    return q;
}

Quaternion LookAtQuaternion(const Vector3& from, const Vector3& to, const Vector3& up) {
    // from から to への方向ベクトルを計算
    Vector3 forward = Normalize(to - from);

    // right ベクトルを計算（up と forward の外積）
    Vector3 right = Normalize(Cross(up, forward));

    // 新しい up ベクトルを計算
    Vector3 newUp = Cross(forward, right);

    // LookAt 行列を設定
    Matrix4x4 lookAtMatrix;
    lookAtMatrix.m[0][0] = right.x;
    lookAtMatrix.m[0][1] = right.y;
    lookAtMatrix.m[0][2] = right.z;
    lookAtMatrix.m[0][3] = 0.0f;

    lookAtMatrix.m[1][0] = newUp.x;
    lookAtMatrix.m[1][1] = newUp.y;
    lookAtMatrix.m[1][2] = newUp.z;
    lookAtMatrix.m[1][3] = 0.0f;

    lookAtMatrix.m[2][0] = forward.x;
    lookAtMatrix.m[2][1] = forward.y;
    lookAtMatrix.m[2][2] = forward.z;
    lookAtMatrix.m[2][3] = 0.0f;

    lookAtMatrix.m[3][0] = 0.0f;
    lookAtMatrix.m[3][1] = 0.0f;
    lookAtMatrix.m[3][2] = 0.0f;
    lookAtMatrix.m[3][3] = 1.0f;

    // 行列をクォータニオンに変換
    return MatrixToQuaternion(lookAtMatrix);
}

Vector3 QuaternionToForward(const Quaternion& quat) {
    // クォータニオンから前方向ベクトルを計算
    // 通常、前方向はz軸を指すため、基準ベクトル(0, 0, 1)に回転を適用します
    float x = 2 * (quat.x * quat.z + quat.w * quat.y);
    float y = 2 * (quat.y * quat.z - quat.w * quat.x);
    float z = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
    return Vector3(x, y, z);
}