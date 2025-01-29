#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <vector>
#include <list>
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"

/// <summary>
///  Vector2 を JSON に変換するときの処理
/// </summary>
inline void to_json(nlohmann::json& j, const Vector2& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y} };
}

/// <summary>
///  JSON から Vector2 を復元するときの処理
/// </summary>
inline void from_json(const nlohmann::json& j, Vector2& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
}

/// <summary>
///  Vector3 を JSON に変換するときの処理
/// </summary>
inline void to_json(nlohmann::json& j, const Vector3& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

/// <summary>
///  JSON から Vector3 を復元するときの処理
/// </summary>
inline void from_json(const nlohmann::json& j, Vector3& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}

/// <summary>
///  Quaternion を JSON に変換するときの処理
/// </summary>
inline void to_json(nlohmann::json& j, const Quaternion& q)
{
    j = nlohmann::json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

/// <summary>
///  JSON から Quaternion を復元するときの処理
/// </summary>
inline void from_json(const nlohmann::json& j, Quaternion& q)
{
    j.at("x").get_to(q.x);
    j.at("y").get_to(q.y);
    j.at("z").get_to(q.z);
    j.at("w").get_to(q.w);
}

/// <summary>
///  登録変数を抽象化するインターフェース
/// </summary>
class IVariable
{
public:
    virtual ~IVariable() = default;

    /// <summary>
    ///  JSON への保存処理
    /// </summary>
    /// <param name="j">書き込み先 JSON オブジェクト</param>
    virtual void SaveToJson(nlohmann::json& j) const = 0;

    /// <summary>
    ///  JSON からの読み込み処理
    /// </summary>
    /// <param name="j">読み込み元 JSON オブジェクト</param>
    virtual void LoadFromJson(const nlohmann::json& j) = 0;
};

/// <summary>
///  具体的な型 T に対する登録変数クラス
/// </summary>
/// <typeparam name="T">変数の型</typeparam>
template <typename T>
class Variable : public IVariable
{
public:
    /// <summary>
    ///  コンストラクタ
    /// </summary>
    /// <param name="ptr">登録対象の変数ポインタ</param>
    Variable(T* ptr)
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

/// <summary>
///  JSON を使って登録した変数を一括管理するクラス
/// </summary>
class JsonManager
{
public:
    /// <summary>
    ///  コンストラクタ
    /// </summary>
    /// <param name="fileName">保存するファイル名（例: "data.json"）</param>
    /// <param name="folderPath">保存先フォルダへのパス（末尾に'/'や'\\'は不要でも良い）</param>
    JsonManager(const std::string& fileName, const std::string& folderPath)
        : fileName_(fileName), folderPath_(folderPath)
    {
        // マネージャ生成時に自動でロードを実行
        LoadAll();
    }

    /// <summary>
    ///  デストラクタ
    /// </summary>
    ~JsonManager() = default;

    /// <summary>
    ///  変数を登録する
    /// </summary>
    /// <typeparam name="T">登録する変数の型</typeparam>
    /// <param name="name">JSON 内でのキーとなる文字列</param>
    /// <param name="ptr">登録する変数のポインタ</param>
    template <typename T>
    void Register(const std::string& name, T* ptr)
    {
        variables_[name] = std::make_unique<Variable<T>>(ptr);
        LoadAll();
    }

    /// <summary>
    ///  登録した変数をすべて保存する
    /// </summary>
    void Save()
    {
        nlohmann::json jsonData;

        // 全変数を JSON に書き込む
        for (auto& pair : variables_)
        {
            const std::string& name = pair.first;
            auto& variablePtr = pair.second;
            variablePtr->SaveToJson(jsonData[name]);
        }

        // フルパス生成（フォルダパス + "/" + ファイル名）
        std::string fullPath = MakeFullPath(folderPath_, fileName_);

        // ファイルに書き出し
        std::ofstream ofs(fullPath);
        if (!ofs)
        {
            std::cerr << "ファイルを開けませんでした: " << fullPath << std::endl;
            return;
        }
        ofs << jsonData.dump(4); // インデント4で整形して出力
        ofs.close();
    }

    /// <summary>
    ///  登録した変数をすべて読み込む
    /// </summary>
    void LoadAll()
    {
        std::string fullPath = MakeFullPath(folderPath_, fileName_);
        std::ifstream ifs(fullPath);
        if (!ifs)
        {
            // ファイルが存在しない場合などは何もしない（新規作成扱い）
            return;
        }

        // ファイルサイズをチェックするために末尾にシーク
        ifs.seekg(0, std::ios::end);
        std::streampos fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        // ファイルサイズが 0（空）なら、新規ファイルとして扱う
        if (fileSize == 0)
        {
            // いったん閉じる
            ifs.close();

            // 登録された変数で Save()（=「初期値をJSONとして書き出し」）し、終了
            Save();
            return;
        }

        // JSON として読み込み
        nlohmann::json jsonData;
        ifs >> jsonData;
        ifs.close();

        // JSON から各変数に反映
        for (auto& pair : variables_)
        {
            const std::string& name = pair.first;
            auto& variablePtr = pair.second;
            if (jsonData.contains(name))
            {
                variablePtr->LoadFromJson(jsonData[name]);
            }
        }
    }

private:
    /// <summary>
    ///  フォルダパスとファイル名からフルパス文字列を組み立てる
    /// </summary>
    /// <param name="folder">フォルダパス</param>
    /// <param name="file">ファイル名</param>
    /// <returns>フルパス文字列</returns>
    std::string MakeFullPath(const std::string& folder, const std::string& file) const
    {
        // fileName_ に拡張子 .json が付いていない場合は付与する
        std::string finalFileName = file;
        static const std::string extension = ".json";
        if (finalFileName.size() < extension.size() ||
            finalFileName.compare(finalFileName.size() - extension.size(), extension.size(), extension) != 0)
        {
            finalFileName += extension;
        }

        // フォルダパスが空なら、ファイル名だけ返す
        if (folder.empty())
        {
            return finalFileName;
        }

        // フォルダパス末尾に '/' or '\\' がなければ追加
        char lastChar = folder[folder.size() - 1];
        if (lastChar == '/' || lastChar == '\\')
        {
            return folder + finalFileName;
        }
        else
        {
            return folder + "/" + finalFileName;
        }
    }

private:
    // 保存先のファイル名
    std::string fileName_;
    // 保存先のフォルダパス
    std::string folderPath_;

    // 登録名 -> 変数オブジェクト
    std::unordered_map<std::string, std::unique_ptr<IVariable>> variables_;
};
