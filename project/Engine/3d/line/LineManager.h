#pragma once

// C++
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <vector>
#include <random>
#include <list>
#include <unordered_map >

// Math
#include "Vector"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Mathfunc.h"

class DirectXCommon;
class SrvManager;
class LineManager
{

public: // メンバ関数

	static LineManager* GetInstance();
	LineManager() = default;
	~LineManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画設定
	/// </summary>
	void SetCommandListConfig();

private:

	/// <summary>
	///  ルートシグネチャ生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	void SetGraphicsPipeline();

	/// <summary>
	/// ルートシグネチャをコマンドリストに積む
	/// </summary>
	void SetRootSignature();
	
	/// <summary>
	/// トポロジーをコマンドリストに積む
	/// </summary>
	void SetPrimitiveTopology();
	
	


private: // メンバ変数

	// シングルトンインスタンス
	static std::unique_ptr<LineManager> instance;
	static std::once_flag initInstanceFlag;

	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	// パイプライン付近関連
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing_[1] = {};

	// ルートパラメーター
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	// サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};

	// ブレンド
	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// SRVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;



};

