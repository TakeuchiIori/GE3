#pragma once
#include "bases/DirectXCommon.h"
#include <memory>
#include <mutex>

class SpriteCommon
{
public: // メンバ関数
    /// <summary>
    /// シングルトンのインスタンスを取得
    /// </summary>
    static SpriteCommon* GetInstance();


    // コンストラクタ
    SpriteCommon() = default;
    ~SpriteCommon() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>
    /// ルートシグネチャの作成
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// グラフィックパイプラインの作成
    /// </summary>
    void CreateGraphicsPipeline();

    /// <summary>
    /// 共通描画設定
    /// </summary>
    void DrawPreference();

    /// <summary>
    /// ルートシグネチャをセット
    /// </summary>
    void SetRootSignature();

    /// <summary>
    /// グラフィックスパイプラインをセット
    /// </summary>
    void SetGraphicsCommand();

    /// <summary>
    /// プリミティブトポロジーをセット
    /// </summary>
    void SetPrimitiveTopology();

public: // アクセッサ

    DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
    // シングルトンインスタンス
    static std::unique_ptr<SpriteCommon> instance;
    static std::once_flag initInstanceFlag;

    // コピーコンストラクタと代入演算子を削除
    SpriteCommon(SpriteCommon&) = delete;
    SpriteCommon& operator=(const SpriteCommon&) = delete;

    DirectXCommon* dxCommon_;
    // ディスクリプターレンジ
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    // ルートシグネチャ
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    // RootParameter作成。複数設定できるので配列。
    D3D12_ROOT_PARAMETER rootParameters[7] = {};
    // Samplerの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    // InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

    D3D12_BLEND_DESC blendDesc{};
    D3D12_RASTERIZER_DESC rasterrizerDesc{};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
};
