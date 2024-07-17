#pragma once
#include "DirectXCommon.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
class SpriteCommon;
class Sprite
{
public: // 構造体
	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	// 座標変換データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 頂点リソース
	/// </summary>
	void VertexResource();

	/// <summary>
	/// 頂点
	/// </summary>
	void CreateVertex();

	/// <summary>
	/// インデックスリソース
	/// </summary>
	void IndexResource();

	/// <summary>
	/// 頂点インデックス
	/// </summary>
	void CreateIndex();

	/// <summary>
	/// マテリアルリソース
	/// </summary>
	void MaterialResource();

	/// <summary>
	/// 座標変換リソース
	/// </summary>
	void TransformResource();

private: // メンバ関数

	SpriteCommon* spriteCommon_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// インデックス用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	// バッファービュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	// データを指すポインタ
	Material* materialData_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	// データを書き込む
	TransformationMatrix* transformationMatrixData_ = nullptr;

};

