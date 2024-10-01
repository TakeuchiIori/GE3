#pragma once
#include <string>
#include <map>
#include <memory>
#include "Model.h"
#include "ModelCommon.h"
#include "DirectXCommon.h"
class ModelManager
{
public: // シングルトン
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 終了
	void Finalize();

public: // 公開メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialze(DirectXCommon* dxCommon);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filaPath"></param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	Model* FindModel(const std::string& filePath);

private:
	static ModelManager* instance;
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator = (ModelManager&) = delete;
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;
private: // メンバ関数
	ModelCommon* modelCommon_ = nullptr;
};

