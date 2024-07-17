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


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

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

private: // メンバ関数

	SpriteCommon* spriteCommon_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// インデックス用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	// バッファービュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
};

