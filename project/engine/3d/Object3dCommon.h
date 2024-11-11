#pragma once
#include "DirectXCommon.h"
#include "Camera.h"
#include <memory>
#include <mutex>

// 3Dオブジェクト共通部
class DirectXCommon;
class Object3dCommon
{
public: // メンバ関数
    // シングルトンインスタンスの取得
    static Object3dCommon* GetInstance();

    // 終了処理
    void Finalize();

    // コンストラクタとデストラクタ
    Object3dCommon() = default;
    ~Object3dCommon() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>
    /// 共通部描画設定
    /// </summary>
    void DrawPreference();

public: // アクセッサ
    // getter
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    // setter
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
    // 線描画用パイプラインステートを取得する
    ID3D12PipelineState* GetLinePipelineState() const { return linePipelineState_.Get(); }
private:
    /// <summary>
    /// ルートシグネチャの作成
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// グラフィックスパイプラインの生成
    /// </summary>
    void CreateGraphicsPipeline();

    /// <summary>
    /// 
    /// </summary>
    void CreateLineRootSignature();

    void CreateLinePipelineState();

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

private:
    // シングルトンインスタンス
    static std::unique_ptr<Object3dCommon> instance;
    static std::once_flag initInstanceFlag;

    // コピーコンストラクタと代入演算子を削除
    Object3dCommon(Object3dCommon&) = delete;
    Object3dCommon& operator=(Object3dCommon&) = delete;

    // DirectX共通クラスのポインタ
    DirectXCommon* dxCommon_;
    // デフォルトカメラのポインタ
    Camera* defaultCamera_ = nullptr;

    // ディスクリプターレンジ
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    // ルートシグネチャの説明構造体
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    // RootParameter配列
    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    // 静的サンプラーの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    // 入力レイアウトの説明
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

    // ブレンドステートの設定
    D3D12_BLEND_DESC blendDesc{};
    // ラスタライザーステートの設定
    D3D12_RASTERIZER_DESC rasterrizerDesc{};
    // デプスステンシルステートの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

    // 頂点シェーダーとピクセルシェーダーのバイナリデータ
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;

    // 入力レイアウトの説明構造体
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

    // ルートシグネチャとグラフィックパイプラインステートのポインタ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr; 
    Microsoft::WRL::ComPtr<ID3D12RootSignature> lineRootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> linePipelineState_ = nullptr;
};
