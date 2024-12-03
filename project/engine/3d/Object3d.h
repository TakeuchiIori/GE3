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
		float shininess;
	};
	// 平行光源
	struct DirectionalLight {
		Vector4 color;		// ライトの色
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
		int32_t enableDirectionalLight; // フラグ
	};
	// 座標変換データ
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverse;
	};

	// カメラのワールド座標を設定
	struct CameraForGPU {
		Vector3 worldPosition;
		int32_t enableSpecular;
		int32_t isHalfVector;
	};

	// ポイントライト
	struct PointLight {
		Vector4 color;	   // 色
		Vector3 position;  // 位置
		float intensity;   // 輝度
		int32_t enableDirectionalLight; // フラグ

		float radius; // ライトの届く最大距離
		float decay;  // 減衰率
		float padding[2];
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
	/// </summary>
	/// マテリアルリソース
	void MaterialResource();

	/// <summary>
	/// 平行光源リソース
	/// </summary>
	void DirectionalLightResource();

	/// <summary>
	/// 鏡面反射リソース
	/// </summary>
	void SpecularReflectionResource();

	/// <summary>
	/// ポイントライトリソース
	/// </summary>
	void PointLightResource();

public: // アクセッサ

	/*===============================================//
					　	 色を変更
	//===============================================*/
	const Vector4& GetColor()const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

	/*===============================================//
					　アンカーポイント
	//===============================================*/
	const Vector3& GetAnchorPoint()const { return anchorPoint_; }
	void SetAnchorPoint(const Vector3& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	/*===============================================//
					　    フリップ
	//===============================================*/
	const bool& GetIsFlipX()const { return isFlipX_; }
	const bool& GetIsFlipY()const { return isFlipY_; }
	void SetIsFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	void SetIsFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

	/*===============================================//
				　   	  カメラ
	//===============================================*/
	void SetModel(const std::string& filePath);
	void ShowLightingEditor();
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	/*===============================================//
		　   			 マテリアル
	//===============================================*/
	const Vector4& GetMaterialColor() const { return materialData_->color; }
	void SetMaterialColor(const Vector4& color) { materialData_->color = color; }
	bool IsLightingEnabled() const { return materialData_->enableLighting != 0; }
	void SetLightingEnabled(bool enabled) { materialData_->enableLighting = enabled ? 1 : 0; }
	float GetMaterialShininess() const { return materialData_->shininess; }
	void SetMaterialShininess(float shininess) { materialData_->shininess = shininess; }
	const Matrix4x4& GetMaterialUVTransform() const { return materialData_->uvTransform; }
	void SetMaterialUVTransform(const Matrix4x4& uvTransform) { materialData_->uvTransform = uvTransform; }

	/*===============================================//
			　   	    平行光源
	//===============================================*/
	const Vector3& GetLightDirection() const { return directionalLight_->direction; }
	void SetLightDirection(const Vector3& direction) { directionalLight_->direction = direction; }
	const Vector4& GetLightColor() const { return directionalLight_->color; }
	void SetLightColor(const Vector4& color) { directionalLight_->color = color; }
	float GetLightIntensity() const { return directionalLight_->intensity; }
	void SetLightIntensity(float intensity) { directionalLight_->intensity = intensity; }
	bool IsDirectionalLight() const { return directionalLight_->enableDirectionalLight != 0; }
	void SetDirectionalLightEnabled(bool enabled) { directionalLight_->enableDirectionalLight = enabled ? 1 : 0; }
	/*===============================================//
					鏡面反射の有効/無効
	//===============================================*/
	bool IsSpecularEnabled() const { return cameraData_->enableSpecular != 0; }
	void SetSpecularEnabled(bool enabled) { cameraData_->enableSpecular = enabled ? 1 : 0; }
	bool IsHalfVector() const { return cameraData_->isHalfVector != 0; }
	void SetHalfVector(bool halfVector) { cameraData_->isHalfVector = halfVector ? 1 : 0; }

	/*===============================================//
	//                 ポイントライト
	//===============================================*/
	const Vector4& GetPointLightColor() const { return pointLight_->color; }
	void SetPointLightColor(const Vector4& color) { pointLight_->color = color; }
	const Vector3& GetPointLightPosition() const { return pointLight_->position; }
	void SetPointLightPosition(const Vector3& position) { pointLight_->position = position; }
	float GetPointLightIntensity() const { return pointLight_->intensity; }
	void SetPointLightIntensity(float intensity) { pointLight_->intensity = intensity; }
	bool IsPointLightEnabled() const { return pointLight_->enableDirectionalLight != 0; }
	void SetPointLightEnabled(bool enabled) { pointLight_->enableDirectionalLight = enabled ? 1 : 0; }
	float GetRadius()const { return pointLight_->radius; }
	void SetRadius(const float radius) { pointLight_->radius = radius; }
	float GetDecay()const { return pointLight_->decay; }
	void SetDecay(const float decay) { pointLight_->decay = decay; }

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

	// 鏡面反射ss
	Microsoft::WRL::ComPtr<ID3D12Resource> specularReflectionResource_;
	CameraForGPU* cameraData_ = nullptr;

	// ポイントライト
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	PointLight* pointLight_= nullptr;

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

