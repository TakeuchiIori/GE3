#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <chrono>
#include "WinApp.h"
#include "DirectXTex.h"


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

	/// <summary>
	/// ログ
	/// </summary>
	/// <param name="message"></param>
	static void Log(const std::string& message);
	static std::wstring ConvertString(const std::string& str);
	static std::string ConvertString(const std::wstring& str);

public: // 描画関連
	// 描画前処理
	void PreDraw();
	// 描画後処理
	void PostDraw();

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
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	/// <summary>
	/// 指定番号のGPUの取得
	/// </summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	
	/// <summary>
	/// SRVの指定番号のCPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	
	/// <summary>
	/// SRVの指定番号のGPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// DSVの指定番号のCPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// DSVの指定番号のGPUディスクリプタハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

private:
	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

	// 記録時間（FPS固定用）
	std::chrono::steady_clock::time_point reference_;

public:
	/// <summary>
	/// シェーダーのコンパイル
	/// </summary>
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const wstring& filePath, const wchar_t* profile);

	/// <summary>
	/// バッファリソースの生成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t  sizeInBytes);

	/// <summary>
	/// テクスチャリソースの生成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	
	/// <summary>
	/// テクスチャデータの転送
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	
	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name ="failePath">テクスチャファイルのパス</param>
	/// <reeturns>画像イメージデータ</returns>
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

public: // アクセッサ
	
	Microsoft::WRL::ComPtr<ID3D12Device> Getdevice() { return device_; }
	Microsoft::WRL::ComPtr<IDxcUtils> GetdxcUtils() { return dxcUtils_; }
	Microsoft::WRL::ComPtr<IDxcCompiler3> GetdxcCompiler() { return dxcCompiler_; }
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> GetincludeHandler() { return includeHandler_; }

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetcommandList() { return commandList_; }
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetcommandQueue() { return commandQueue_; }
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetcommandAllocator() { return commandAllocator_; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetrtvDescriptorHeap() const { return rtvDescriptorHeap_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetsrvDescriptorHeap() const { return srvDescriptorHeap_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetdsvDescriptorHeap() const { return dsvDescriptorHeap_; }

	uint32_t GetdescriptotSizeSRV() const { return descriptotSizeSRV_;}
	uint32_t GetdescriptotSizeRTV() const { return descriptotSizeRTV_; }
	uint32_t GetdescriptotSizeDSV() const { return descriptotSizeDSV_; }

	Microsoft::WRL::ComPtr<IDXGISwapChain4> GetswapChain() { return swapChain_; }
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> GetswapChainResources() { return swapChainResources_; }


	D3D12_CPU_DESCRIPTOR_HANDLE* GetrtvHandles() { return rtvHandles_; }

	Microsoft::WRL::ComPtr<ID3D12Fence> Getfence() { return fence_; }
	HANDLE GetfenceEvent() { return fenceEvent_; }
	uint64_t GetfenceValue() { return fenceValue_; }
	uint64_t SetfenceValue(uint64_t val) {  return fenceValue_ = val; }





	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxSRVCount;
private: // メンバ変数

	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	// DXCコンパイラ関連
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;

	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	// フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>,2> swapChainResources_;
	// スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
																  
	// 現時点ではincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	uint32_t descriptotSizeSRV_;
	uint32_t descriptotSizeRTV_;
	uint32_t descriptotSizeDSV_;
	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	// RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	

	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;
	// ビューポート
	D3D12_VIEWPORT viewport_{};
	// シザー矩形
	D3D12_RECT scissorRect_{};
	

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};


};
