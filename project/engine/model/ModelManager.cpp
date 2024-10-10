#include "ModelManager.h"
std::unique_ptr<ModelManager> ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance == nullptr) {
		instance = std::make_unique<ModelManager>();
	}
	return instance.get();
}

void ModelManager::Finalize()
{
	instance.reset();  // インスタンスを解放
}

void ModelManager::Initialze(DirectXCommon* dxCommon)
{
	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath)
{
	// 読み込ん済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}
	// モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelCommon_.get(), "Resources", filePath);
	
	// モデルをmapコンテナに格納する
	models.insert(std::make_pair(filePath, std::move(model))); // 所有権を譲渡

}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}
	// ファイル名一致無し
	return nullptr;
}
