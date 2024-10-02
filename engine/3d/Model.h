#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "VectorSRT.h"
#include "DirectXCommon.h"
class ModelCommon;

class Model
{
public: // 構造体
	// 頂点データ
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
	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	// 平行光源
	struct DirectionalLight {
		Vector4 color;		// ライトの色
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
	};
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath,const std::string& filename);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private:
	/// <summary>
	/// 頂点リソース
	/// </summary>
	void VertexResource();

	/// <summary>
	/// 頂点
	/// </summary>
	void CreateVertex();

	/// <summary>
	/// マテリアルリソース
	/// </summary>
	void MaterialResource();


	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

public: // アクセッサ



private: // メンバ変数
	// ModelCommonのポインタ
	ModelCommon* modelCommon_;
	// objファイルのデータ
	ModelData modelData_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// バッファリソースの使い道を補足するバッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	
	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;
	
	
};

