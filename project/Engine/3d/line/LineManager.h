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
private:

	/// <summary>
	///  ルートシグネチャ生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreateGraphicsPipeline();
	
	

public:

	ID3D12RootSignature* GetRootSignature_() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPiplineState() { return graphicsPipelineState_.Get(); }


private: 

	// シングルトンインスタンス
	static std::unique_ptr<LineManager> instance;
	static std::once_flag initInstanceFlag;

	// 外部からのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;



	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

};
