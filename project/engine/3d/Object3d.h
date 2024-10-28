#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Camera.h"



//前方宣言
class WorldTransform;
class Object3dCommon;
class Model;
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
		Matrix4x4 WorldInverse;
	};

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(WorldTransform& worldTransform);

	/// <summary>
	/// テクスチャ変更
	/// </summary>
	void ChangeTexture(std::string textureFilePath);

	
private:

	/// <summary>
	/// 平行光源リソース
	/// </summary>
	void DirectionalLightResource();

	/// <summary>
	/// マテリアルリソース
	/// </summary>
	void MaterialResource();

public: // アクセッサ

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

	
	void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera) { this->camera_ = camera; }

private: // メンバ変数
	// ポインタ
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Material* materialData_ = nullptr;

	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	// 平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLight_ = nullptr;
	

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };

	
	

	// アンカーポイント
	Vector3 anchorPoint_ = { 0.5f,0.5f,0.5f };
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;
};

