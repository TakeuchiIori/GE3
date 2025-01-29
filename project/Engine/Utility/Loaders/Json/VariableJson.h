#pragma once
#include <json.hpp>
#include <memory>

/// <summary>
///  登録変数を抽象化するインターフェース
/// </summary>
class IVariableJson
{
public:
    virtual ~IVariableJson() = default;

    virtual void SaveToJson(nlohmann::json& j) const = 0;
    virtual void LoadFromJson(const nlohmann::json& j) = 0;
};

template <typename T>
class VariableJson : public IVariableJson
{
public:
    /// <summary>
    ///  コンストラクタ
    /// </summary>
    /// <param name="ptr">登録対象の変数ポインタ</param>
    VariableJson(T* ptr)
        : ptr_(ptr)
    {
    }

    /// <summary>
    ///  JSON への保存処理
    /// </summary>
    /// <param name="j">書き込み先 JSON オブジェクト</param>
    void SaveToJson(nlohmann::json& j) const override
    {
        j = *ptr_;
    }

    /// <summary>
    ///  JSON からの読み込み処理
    /// </summary>
    /// <param name="j">読み込み元 JSON オブジェクト</param>
    void LoadFromJson(const nlohmann::json& j) override
    {
        if (!j.is_null())
        {
            *ptr_ = j.get<T>();
        }
    }

private:
    T* ptr_;
};
