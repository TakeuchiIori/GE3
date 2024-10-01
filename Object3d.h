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
	/// 座標変換行列の初期化
	/// </summary>
	void TransformationInitialize();

	/// <summary>
	/// 平行光源リソース
	/// </summary>
	void DirectionalLightResource();



public: // アクセッサ

	/*===============================================//
						  座標
	//===============================================*/
	const Vector3& GetPosition()const { return position_; }
	void SetPosition(const Vector3& position) { position_ = position; }

	/*===============================================//
						  回転
	//===============================================*/
	const Vector3& GetRotation()const { return rotation_; }
	void SetRotation(const Vector3& rotation) { rotation_ = rotation; }

	/*===============================================//
						  拡縮
	//===============================================*/
	const Vector3& GetScale() { return scale_; }
	void Setscale(const Vector3& scale) { scale_ = scale; }

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

	void SetModel(Model* model) { this->model_ = model; }

private: // メンバ変数
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;

	// 平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLight_ = nullptr;
	// 座標変換
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	TransformationMatrix* transformationMatrixData_ = nullptr;


	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };

	// 3DオブジェクトのSRT
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	Vector3 position_ = { 0.0f,3.0f,0.0f};
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	

	// アンカーポイント
	Vector3 anchorPoint_ = { 0.5f,0.5f,0.5f };
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	VectorSRT transform_;
	VectorSRT cameraTransform_;

};

