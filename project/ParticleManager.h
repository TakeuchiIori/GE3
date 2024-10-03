#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <chrono>
#include <vector>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"


class DirectXCommon;
class SrvManager;
class ParticleManager {

public:
	enum BlendMode {
		// ブレンド無し
		kBlendModeNone,
		// 通常のブレンド
		kBlendModeNormal,
		// 加算
		kBlendModeAdd,
		// 減算
		kBlendModeSubtract,
		// 乗算
		kBlendModeMultiply,
		// スクリーン
		kBlendModeScreen,

		// 利用してはいけない
		kCount0fBlendMode,
	};
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};
public:
    // シングルトンインスタンスの取得
    static ParticleManager* GetInstance();

    // コピーコンストラクタと代入演算子を削除して、複製を防ぐ
        // コンストラクタをプライベートにして、外部からの直接生成を防ぐ
    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetBlendMode(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode);

	void ShowBlendModeDropdown(BlendMode& currentBlendMode);

	void Render(D3D12_BLEND_DESC& blendDesc, BlendMode& currentBlendMode);

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
	/// 頂点リソース
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// 頂点
	/// </summary>
	void CreateVertexVBV();




private: // メンバ変数
	DirectXCommon* dxCommon_;
	SrvManager* srvManager_;

	// ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	// ルートパラメーター
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	// サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	// ブレンド
	D3D12_BLEND_DESC blendDesc{};

	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;



	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

};
