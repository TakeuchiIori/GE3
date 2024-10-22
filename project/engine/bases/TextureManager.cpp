#include "TextureManager.h"
#include "assert.h"

TextureManager* TextureManager::instance = nullptr;
// ImGUiで0番を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;
TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;

}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	// SRVの数と同数のバケット数を確保
	textureDatas.reserve(SrvManager::kMaxSRVCount_);
	 // dxCommon_ を設定
    dxCommon_ = dxCommon;
	// SrvManagerを設定
	srvManager_ = srvManager;

}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}
	// テクスチャ上限枚数チェック
	assert(srvManager_->IsAllocation());


	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filepathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filepathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// テクスチャデータを追加
	TextureData& textureData = textureDatas[filePath];
	
	textureData.srvIndex = srvManager_->Allocate();
	textureData.metadata = image.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	// テクスチャデータをGPUにアップロード
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource.Get(), image);


	// テクスチャデータの要素番号をSRVのインデックスとする
	//uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	textureData.srvHandleCPU = srvManager_->GetCPUSRVDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUSRVDescriptorHandle(textureData.srvIndex);

	/*===============================================//
						SRVの生成
	//===============================================*/

	// srvManagerを使用してSRVを生成
	srvManager_->CreateSRVforTexture2D(
    textureData.srvIndex,                       // SRVのインデックス
    textureData.resource.Get(),                 // リソース
    textureData.metadata.format,                // フォーマット
    UINT(textureData.metadata.mipLevels)        // ミップレベル
);
	
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// unordered_mapを使って直接インデックスを取得
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end()) {
		return it->second.srvIndex;
	}
	Log("Error: Texture not found for filePath: " + filePath); // ログを追加
	// 見つからない場合はassertでエラーにする
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetsrvHandleGPU(const std::string& filePath)
{ 
	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// テクスチャが存在しない場合はエラーハンドリング
		Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;

	// GPUハンドルを返却
	return textureData.srvHandleGPU;
}

std::wstring TextureManager::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string TextureManager::ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
void TextureManager::Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{

	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// テクスチャが存在しない場合はエラーハンドリング
		Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;

	// メタデータを返却
	return textureData.metadata;
}
