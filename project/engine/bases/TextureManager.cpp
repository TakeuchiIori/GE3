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
	 // dxCommon_ を設定
    this->dxCommon_ = dxCommon;
	// SrvManagerを設定
	this->srvManager_ = srvManager;

	// SRVの数と同数
	textureDatas.reserve(SrvManager::kMaxSRVcount_);
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

	// 追加したテクスチャデータの参照を取得する
	//TextureData& textureData = textureDatas.back();
	
	textureData.metadata = image.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	// テクスチャデータをGPUにアップロード
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource.Get(), image);


	// テクスチャデータの要素番号をSRVのインデックスとする
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUSRVDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUSRVDescriptorHandle(textureData.srvIndex);

	/*===============================================//
						SRVの生成
	//===============================================*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	// SRVの生成
	dxCommon_->Getdevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		// 読み込み済みなら早期return
		return 0;
	}

}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetsrvHandleGPU(const std::string& filePath)
{ 
    // テクスチャ番号が範囲内であることを確認
  //  assert(filePath < textureDatas.size());

    // テクスチャデータの参照を取得
    TextureData& textureData = textureDatas[filePath];

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

	// テクスチャ番号が範囲内であることを確認
	//assert(textureIndex < textureDatas.size());

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas[filePath];

	// メタデータを返却
	return textureData.metadata;
}
