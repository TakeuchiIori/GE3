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

	// コンストラクタ
	// デストラクタ
	ModelManager() = default;
	~ModelManager() = default;

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
	static std::unique_ptr<ModelManager> instance;  // std::unique_ptrを使用
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;
private: // メンバ関数
	std::unique_ptr<ModelCommon> modelCommon_;
};

