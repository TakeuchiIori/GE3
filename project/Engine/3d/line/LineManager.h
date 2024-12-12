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
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Mathfunc.h"

class DirectXCommon;
class SrvManager;
class LineManager
{

public: // メンバ関数
	// コンストラクタとデストラクタ
	LineManager() = default;
	~LineManager() = default;

	static LineManager* GetInstance();
	LineManager(const LineManager&) = delete;
	LineManager& operator=(const LineManager&) = delete;
	LineManager(LineManager&&) = delete;
	LineManager& operator=(LineManager&&) = delete;

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

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGraphicsPiplineState() { return graphicsPipelineState_.Get(); }


private: 

	// 外部からのポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

};

