/// <summary>
///  CustomTypes.h
///  Vector2, Vector3, Quaternion などのカスタム構造体
///  nlohmann::json 用のシリアライズ( to_json ) / デシリアライズ( from_json ) をまとめる
/// </summary>
#pragma once

#include <json.hpp>
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

/// <summary>
///  Vector2 の to_json 実装
/// </summary>
inline void to_json(nlohmann::json& j, const Vector2& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y} };
}

/// <summary>
///  Vector2 の from_json 実装
/// </summary>
inline void from_json(const nlohmann::json& j, Vector2& v)
{
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
}

/// <summary>
///  Vector3 の to_json 実装
/// </summary>
inline void to_json(nlohmann::json& j, const Vector3& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

/// <summary>
///  Vector3 の from_json 実装
/// </summary>
inline void from_json(const nlohmann::json& j, Vector3& v)
{
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
}

/// <summary>
///  Quaternion の to_json 実装
/// </summary>
inline void to_json(nlohmann::json& j, const Quaternion& q)
{
    j = nlohmann::json{ {"w", q.w}, {"x", q.x}, {"y", q.y}, {"z", q.z} };
}

/// <summary>
///  Quaternion の from_json 実装
/// </summary>
inline void from_json(const nlohmann::json& j, Quaternion& q)
{
    q.w = j.at("w").get<float>();
    q.x = j.at("x").get<float>();
    q.y = j.at("y").get<float>();
    q.z = j.at("z").get<float>();
}

namespace nlohmann {
    template <>
    struct adl_serializer<Vector3> {
        static void to_json(json& j, const Vector3& v) {
            j = json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
        }

        static void from_json(const json& j, Vector3& v) {
            v.x = j.at("x").get<float>();
            v.y = j.at("y").get<float>();
            v.z = j.at("z").get<float>();
        }
    };
}