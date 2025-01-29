#pragma once
#include <json.hpp>
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

/// <summary>
///  Vector2 を JSON に変換
/// </summary>
inline void to_json(nlohmann::json& j, const Vector2& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y} };
}

/// <summary>
///  JSON から Vector2 を復元
/// </summary>
inline void from_json(const nlohmann::json& j, Vector2& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
}

/// <summary>
///  Vector3 を JSON に変換
/// </summary>
inline void to_json(nlohmann::json& j, const Vector3& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

/// <summary>
///  JSON から Vector3 を復元
/// </summary>
inline void from_json(const nlohmann::json& j, Vector3& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}

/// <summary>
///  Quaternion を JSON に変換
/// </summary>
inline void to_json(nlohmann::json& j, const Quaternion& q)
{
    j = nlohmann::json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

/// <summary>
///  JSON から Quaternion を復元
/// </summary>
inline void from_json(const nlohmann::json& j, Quaternion& q)
{
    j.at("x").get_to(q.x);
    j.at("y").get_to(q.y);
    j.at("z").get_to(q.z);
    j.at("w").get_to(q.w);
}
