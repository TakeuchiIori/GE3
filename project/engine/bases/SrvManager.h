#pragma once
#include "cstdint"
#include "wrl.h"
#include <d3d12.h>
#include <dxgi1_6.h>
class DirectXCommon;
// SRV管理
class SrvManager
{
	

public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// アロケータ
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();

	/// <summary>
	/// 描画の前準備
	/// </summary>
	void PreDraw();

	/// <summary>
	/// SRVセットコマンド
	/// </summary>
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	/// <summary>
	/// SRVの指定番号のCPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSRVDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVの指定番号のGPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVDescriptorHandle(uint32_t index);

	/// <summary>
	/// 確保可能チェック
	/// </summary>
	bool IsAllocation();

private:
	/// <summary>
	/// SRV生成（テクスチャ用）
	/// SRV生成（Structured Buffer用）
	/// </summary>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT
	structureByteStride);



public:
	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxSRVcount_;

private: // メンバ変数
	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;

	
	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize_;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};

