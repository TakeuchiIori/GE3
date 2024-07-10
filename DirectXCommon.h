#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include "WinApp.h"

/// <summary>
/// DirectX基盤
/// </summary>
using namespace std;
class DirectXCommon
{
public: // 各種初期化
	/// <summary>
	/// DirectXの初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();//bool enableDebugLayer = true
	
	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲットの初期化
	/// </summary>
	void InitializeRenderTarget();

	/// <summary>
	/// 深度ステンシルビューの初期化
	/// </summary>
	void InitializeDepthStencilView();

	/// <summary>
	/// フェンスの初期化
	/// </summary>
	void InitializeFence();

	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void InitializeViewPortRevtangle();
	
	/// <summary>
	/// シザリング矩形の初期化
	/// </summary>
	void InitializeScissorRevtangle();

	/// <summary>
	/// ImGuiの初期化
	/// </summary>
	void InitializeImGui();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// 各種ディスクリプターヒープの生成
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void CreateDXCompiler();

public: // メンバ関数
	/// <summary>
	/// ディスクリプターヒープ
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	
	/// <summary>
	/// 深度バッファのリソース
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);
	
	/// <summary>
	/// 指定番号のCPUの取得
	/// </summary>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	/// <summary>
	/// 指定番号のGPUの取得
	/// </summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	/// <summary>
	/// SRVの指定番号のCPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	
	/// <summary>
	/// SRVの指定番号のGPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);
private: // メンバ変数

	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	// DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	// DXCコンパイラ関連
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;

	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	// フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,2> swapChainResources;

	// RTV用のヒープディスクリプタの数は2。  ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	// SRV用のヒープディスクリプタの数は128。ShaderVisibleはtrue
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	// DSV用のヒープディスクリプタの数は1。  ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

private:
	uint32_t descriptotSizeSRV;
	uint32_t descriptotSizeRTV;
	uint32_t descriptotSizeDSV;
	// RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// シザー矩形
	D3D12_RECT scissorRect{};
};

