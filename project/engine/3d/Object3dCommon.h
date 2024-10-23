#pragma once
#include "DirectXCommon.h"
#include "Camera.h"

// 3Dオブジェクト共通部
class DirectXCommon;
class Object3dCommon
{
public: // メンバ関数
	// シングルトン
	static Object3dCommon* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタ
	// デストラクタ
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
	void SetDefaultCamera(Camera* camera){ this->defaultCamera_ = camera; }


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
	static std::unique_ptr<Object3dCommon> instance;
	Object3dCommon(Object3dCommon&) = delete;
	Object3dCommon& operator=(Object3dCommon&) = delete;

	// ポインタ
	DirectXCommon* dxCommon_;
	Camera* defaultCamera_ = nullptr;
	// ディスクリプターレンジ
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	// ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	// RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
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
};

