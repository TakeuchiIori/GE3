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

// GAME
#include "Vector"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Camera.h"
#include "Mathfunc.h"

class SrvManager;
class DirectXCommon;
class Billboard{
public:

    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };
    struct MaterialData {
        std::string textureFilePath;
        uint32_t textureIndexSRV = 0;
    };
    struct ModelData {
        std::vector<VertexData> vertices;
        MaterialData material;
    };
    struct BillBoadForGPU {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;
    };
    struct Material {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
    };

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update(Camera* camera);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

private:

    /// <summary>
    /// ルートシグネチャの生成
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// グラフィックパイプラインをセット
    /// </summary>
    void CreateGraphicsPipeline();

    /// <summary>
    /// グラフィックパイプラインの設定
    /// </summary>
    void SetGraphicsPipeline();

    /// <summary>
    /// 頂点バッファの生成
    /// </summary>
    void CreateVertexResource();

    /// <summary>
    /// マテリアルリソース
    /// </summary>
    void CreateMaterialResource();

   
private:

    // 他クラスのポインタ
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;

	// ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing_[1] = {};
	// ルートパラメーター
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	// サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};

	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;


	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;


	// SRV切り替え
	bool useTexture = true;
	bool particleUpdate = false;
	bool useBillboard = false;

    const uint32_t kNumMaxInstance = 16; // インスタンス数
    BillBoadForGPU* billboadForGPU_;


    ModelData modelData_;
    VertexData* vertexData_ = nullptr;
    Material* materialData_ = nullptr;

	Matrix4x4 scaleMatrix;
	Matrix4x4 translateMatrix;
};


