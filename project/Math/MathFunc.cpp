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



