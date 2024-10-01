#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "VectorSRT.h"

//　構造体

//前方宣言
class Object3dCommon;
// 3Dオブジェクト
class Object3d
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
	// 座標変換データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Object3dCommon* object3dCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// テクスチャ変更
	/// </summary>
	void ChangeTexture(std::string textureFilePath);

	
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
	void DirectionalLightResource();

	/// <summary>
	/// テクスチャサイズをイメージに合わせる
	/// </summary>
	void AdjustTaxtureSize();

	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

public: // アクセッサ

	/*===============================================//
						  座標
	//===============================================*/
	const Vector3& GetPosition()const { return position_; }
	void SetPosition(const Vector3& position) { position_ = position; }

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

	/*===============================================//
					　アンカーポイント
	//===============================================*/
	// getter
	const Vector3& GetAnchorPoint()const { return anchorPoint_; }
	// setter
	void SetAnchorPoint(const Vector3& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	/*===============================================//
					　    フリップ
	//===============================================*/
	// getter
	const bool& GetIsFlipX()const { return isFlipX_; }
	const bool& GetIsFlipY()const { return isFlipY_; }
	// setter
	void SetIsFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	void SetIsFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }


private: // メンバ変数
	Object3dCommon* object3dCommon_ = nullptr;
	// objファイルのデータ
	ModelData modelData_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;
	// 平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLight_ = nullptr;
	// 座標変換
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	TransformationMatrix* transformationMatrixData_ = nullptr;


	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	// バッファリソースの使い道を補足するバッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };

	// 3DオブジェクトのSRT
	Vector3 position_ = { 0.0f,0.0f,0.0f};
	float rotation_ = 0.0f;
	Vector2 size_ = { 100.0f,100.0f};

	// アンカーポイント
	Vector3 anchorPoint_ = { 0.5f,0.5f,0.5f };
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	VectorSRT transform_;
	VectorSRT cameraTransform_;

};

