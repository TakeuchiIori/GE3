/// <summary>
///  IVariableReference.h
///  変数の保存/読み込みを行うインターフェースクラス
///  各型で個別に実装するための基底となる純粋仮想クラス
/// </summary>
#pragma once

#include <json.hpp>

/// <summary>
///  変数をJSONに対して Save / Load するインターフェースクラス
/// </summary>
class IVariableReference
{
public:
    /// <summary>
    ///  仮想デストラクタ
    /// </summary>
    virtual ~IVariableReference() = default;

    /// <summary>
    ///  JSONに値を保存する純粋仮想関数
    /// </summary>
    /// <param name="j">保存先のJSONオブジェクト</param>
    virtual void Save(nlohmann::json& j) const = 0;

    /// <summary>
    ///  JSONから値を読み込む純粋仮想関数
    /// </summary>
    /// <param name="j">読み込み元のJSONオブジェクト</param>
    virtual void Load(const nlohmann::json& j) = 0;
};
