#pragma once
#include <string>
#include <wrl.h>
#include <vector>
#include "DirectXCommon.h"
#include "d3d12.h"
#include "DirectXTex.h"
#include "SrvManager.h"
#include <unordered_map>

// テクスチャマネージャー
class TextureManager
{
private:
	// テクスチャ1枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public: // メンバ関数

	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	// 終了
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);


	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	void LoadTexture(const std::string& filePath);

	/// <summary>
	/// SRVインデックスの開始番号
	/// </summary>
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	
	/// <summary>
	/// テクスチャ番号からGPUハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetsrvHandleGPU(uint32_t textureIndex);

	std::wstring ConvertString(const std::string& str);
	std::string ConvertString(const std::wstring& str);
	void Log(const std::string& message);

	// メタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private: // メンバ変数

	static TextureManager* instance;
	SrvManager* srvManager_;
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator = (TextureManager&) = delete;

	// テクスチャデータ
	std::unordered_map<std::string,TextureData> textureDatas;
	// DirectXCommon
	DirectXCommon* dxCommon_ = nullptr;


	// テクスチャハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

};

