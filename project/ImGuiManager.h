#pragma once
#include <wrl.h>
#include <d3d12.h>


class WinApp;
class DirectXCommon;
class ImGuiManager
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp,DirectXCommon* dxCommon);

	/// <summary>
	/// デスクリプターヒープ生成
	/// </summary>
	void CreateDescriptorHeap();




private: // メンバ変数
	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	// SRV用デスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

