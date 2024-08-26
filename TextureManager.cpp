#include "TextureManager.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"

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

void TextureManager::Initialize()
{
	// SRVの数と同数
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		// 読み込み済みなら早期return
		return;
	}



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
	textureDatas.resize(textureDatas.size() + 1);
	// 追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas.back();

	textureData.filePath = filePath;
	textureData.metadata = image.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);

	// テクスチャデータの要素番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;

	textureData.srvHandleCPU = dxCommon_->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = dxCommon_->GetSRVGPUDescriptorHandle(srvIndex);

	/*===============================================//
						SRVの生成
	//===============================================*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	
	// SRVを作成DescriptorHeapの場所を決める
	textureSrvHandleCPU = spriteCommon_->GetDxCommon()->GetCPUDescriptorHandle(spriteCommon_->GetDxCommon()->GetsrvDescriptorHeap().Get(), spriteCommon_->GetDxCommon()->GetdescriptotSizeSRV(), 1);
	textureSrvHandleGPU = spriteCommon_->GetDxCommon()->GetGPUDescriptorHandle(spriteCommon_->GetDxCommon()->GetsrvDescriptorHeap().Get(), spriteCommon_->GetDxCommon()->GetdescriptotSizeSRV(), 1);

	// SRVの生成
	spriteCommon_->GetDxCommon()->Getdevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureSrvHandleCPU);
	// 2枚目のSRVの生成



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