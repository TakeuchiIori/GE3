#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "externals/DirectXTex/DirectXTex.h"
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

public: // 基本的関数
	Sprite();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ関数

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

	/// <summary>
	/// Textureを読んで転送
	/// </summary>
	void TransferTexture();

	/// <summary>
	/// SRVの設定
	/// </summary>
	void SetSRV();

public: // アクセッサ

	/*===============================================//
						  座標
	//===============================================*/
	const Vector2& GetPosition()const { return position_; }
	void SetPosition(const Vector2& position) { position_ = position; }

	/*===============================================//
						  回転
	//===============================================*/
	float GetRotation()const { return rotation_; }
	void SetRotation(float rotation) { rotation_ = rotation; }

	/*===============================================//
						  拡縮
	//===============================================*/
	const Vector2& GetSize() { return size_; }
	void SetSize(const Vector2& size) { size_ = size; }

	/*===============================================//
					　	 色を変更
	//===============================================*/
	const Vector4& GetColor()const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }


private: // メンバ関数

	SpriteCommon* spriteCommon_ = nullptr;
	/*===============================================//
						Resouurces
	//===============================================*/

	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	// 座標変換
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	TransformationMatrix* transformationMatrixData_ = nullptr;

	/*===============================================//
						Texture
	//===============================================*/

	DirectX::ScratchImage mipImages[2] = {};
	const DirectX::TexMetadata& metadata = mipImages[0].GetMetadata();
	const DirectX::TexMetadata& metadata2 = mipImages[1].GetMetadata();

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource[2];

	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2;
	//Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	/*===============================================//
						  SRV
	//===============================================*/

	Vector2 position_ = { 0.0f,0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 100.0f,100.0f };




};

