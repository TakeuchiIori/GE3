#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>

// Engine
#include "3d/camera/Camera.h"

// Math
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

class Line;
class WorldTransform;
class Object3dCommon;
class Model;
// 3Dオブジェクト
class Object3d
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(WorldTransform& worldTransform);

	/// <summary>
	/// スケルトン描画
	/// </summary>
	void DrawSkeleton();

	/// <summary>
	/// モデルのセット
	/// </summary>
	void SetModel(const std::string& filePath, bool isAnimation = false);

	/// <summary>
	/// ImGui
	/// </summary>
	void MaterialByImGui();

private:

	/// <summary>
	/// マテリアルリソース作成
	/// </summary>
	void CreateMaterialResource();

public: // アクセッサ
	Model* GetModel() { return model_; }

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

	
	void SetCamera(Camera* camera) { this->camera_ = camera; }
	void SetLine(Line* line) { this->line_ = line; }

	// マテリアル
	const Vector4& GetMaterialColor() const { return materialData_->color; }
	void SetMaterialColor(const Vector4& color) { materialData_->color = color; }
	bool IsLightingEnabled() const { return materialData_->enableLighting != 0; }
	void SetLightingEnabled(bool enabled) { materialData_->enableLighting = enabled ? 1 : 0; }
	float GetMaterialShininess() const { return materialData_->shininess; }
	void SetMaterialShininess(float shininess) { materialData_->shininess = shininess; }
	const Matrix4x4& GetMaterialUVTransform() const { return materialData_->uvTransform; }
	void SetMaterialUVTransform(const Matrix4x4& uvTransform) { materialData_->uvTransform = uvTransform; }
	bool IsMaterialEnabled() const { return materialData_->enableLighting != 0; }
	void SetMaterialEnabled(bool enable) { materialData_->enableLighting = enable; }

private:
	// マテリアルデータ
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};
	// マテリアルのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	// 外部からのポインタ
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Line* line_ = nullptr;

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

