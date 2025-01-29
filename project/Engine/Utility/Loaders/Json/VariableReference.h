/// <summary>
///  VariableReference.h
///  IVariableReference を継承し、
///  任意の型(T)に対して実際に変数参照を管理するテンプレートクラス
/// </summary>
#pragma once
#include "IVariableReference.h"

/// <summary>
///  任意の型 T の変数を保存・読み込みするためのクラス
/// </summary>
template <typename T>
class VariableReference : public IVariableReference
{
public:
    /// <summary>
    ///  コンストラクタ
    ///  <param name="key">JSON上でのキー</param>
    ///  <param name="ptr">参照したい変数へのポインタ</param>
    /// </summary>
    VariableReference(const std::string& key, T* ptr)
        : m_key(key), m_ptr(ptr)
    {
    }

    /// <summary>
    ///  JSONに値を保存する関数
    ///  <param name="j">保存先のJSONオブジェクト</param>
    /// </summary>
    void Save(nlohmann::json& j) const override
    {
        j[m_key] = *m_ptr;
    }

    /// <summary>
    ///  JSONから値を読み込む関数
    ///  <param name="j">読み込み元のJSONオブジェクト</param>
    /// </summary>
    void Load(const nlohmann::json& j) override
    {
        if (j.contains(m_key))
        {
            *m_ptr = j[m_key].get<T>();
        }
    }

private:
    /// <summary>
    ///  JSON上のキー
    /// </summary>
    std::string m_key;

    /// <summary>
    ///  登録された変数へのポインタ
    /// </summary>
    T* m_ptr;
};
