#pragma once
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
#include "Vector"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Camera.h"

class DirectXCommon;
class SrvManager;
class ParticleManager {

public:
	// ブレンドモード構造体
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
		uint32_t textureIndexSRV = 0;
	};
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};
	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	struct Particle {
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};
	struct ParticleGroup {
		MaterialData materialData;										// マテリアルデータ
		std::list<Particle> particles;									// パーティクルリスト
		uint32_t srvIndex;												// インスタンシングデータ用SRVインデックス
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;		// インスタンシングリソース
		UINT instance;													// インスタンス数
		ParticleForGPU* instancingData;									// インスタンシングデータを書き込むためのポインタ
	};

public: // シングルトン
    static ParticleManager* GetInstance();
	void Finalize();
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

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ブレンドモードせてい
	/// </summary>
	/// <param name="blendDesc"></param>
	/// <param name="blendMode"></param>
	void SetBlendMode(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode);

	/// <summary>
	/// ImGuiでブレンドモード変更
	/// </summary>
	/// <param name="currentBlendMode"></param>
	void ShowBlendModeDropdown(BlendMode& currentBlendMode);


	void Render(D3D12_BLEND_DESC& blendDesc, BlendMode& currentBlendMode);

	/// <summary>
	/// パーティクルグループ生成
	/// </summary>
	/// <param name="name"></param>
	/// <param name="textureFilePath"></param>
	void CreateParticleGroup(const std::string name, const std::string textureFilePath);


	/// <summary>
	/// パーティクルの発生
	/// </summary>
	/// <param name="name"></param>
	/// <param name="position"></param>
	/// <param name="count"></param>
	void Emit(const std::string& name, const Vector3& position, uint32_t count);


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
	/// マテリアルリソース
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 頂点バッファビュー
	/// </summary>
	void CreateVertexVBV();

	/// <summary>
	/// ランダムエンジン
	/// </summary>
	void InitRandomEngine();
	
	/// <summary>
	/// パイプラインの設定
	/// </summary>
	void SetGraphicsPipeline();
	
public:
	void SetCamera(Camera* camera) { camera_ = camera; }

private: // メンバ変数

	// シングルトン
	static std::unique_ptr<ParticleManager> instance;
	static std::once_flag initInstanceFlag;

	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;
	Camera* camera_ = nullptr;

	// ルートシグネチャ
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing_[1] = {};
	// ルートパラメーター
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	// サンプラー
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};

	// ブレンド
	BlendMode blendMode{};
	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

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

	ModelData modelData_;
	// 乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine_;
	// 最初のブレンドモード
	BlendMode currentBlendMode_;
	// パーティクルグループコンテナ
	std::unordered_map<std::string, ParticleGroup> particleGroups_;
	const float kDeltaTime = 1.0f / 60.0f;
	// インスタンシング用リソース作成
	const uint32_t kNumMaxInstance = 100; // インスタンス数


	Matrix4x4 scaleMatrix;
	Matrix4x4 translateMatrix;
};
