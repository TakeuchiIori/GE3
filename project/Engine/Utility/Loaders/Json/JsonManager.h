#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include "ConversionJson.h"
#include "VariableJson.h"

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
        variables_[name] = std::make_unique<VariableJson<T>>(ptr);
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
    std::unordered_map<std::string, std::unique_ptr<IVariableJson>> variables_;
};
