#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>


/// <summary>
/// DirectX基盤
/// </summary>

class DirectXCommon
{
public: // メンバ関数
	/// <summary>
	/// DirectXの初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice(bool enableDebugLayer = true);
	
	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

private: // メンバ変数
	// DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	// DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	
};

