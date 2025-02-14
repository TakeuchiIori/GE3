#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>

// Math
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
class DirectXCommon;
class Material
{
public:
	/*=================================================================
	 
								構造体

	=================================================================*/
	struct MaterialData {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
		bool enableSpecular;
		bool isHalfVector;
	};


public:
	/*=================================================================

								メンバ関数

	=================================================================*/	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// データ転送
	/// </summary>
	void TransferData();


public:
	/*=================================================================

								アクセッサ

	=================================================================*/
	
	///
	/// Color 
	/// 
	const Vector4& GetMaterialColor() const { return materialData_->color; }
	void SetMaterialColor(const Vector4& color) { materialData_->color = color; }
	void SetAlpha(const float& alpha) { materialData_->color.w = alpha; }

	///
	/// Lighting
	/// 
	bool IsLightingEnabled() const { return materialData_->enableLighting != 0; }
	void SetLightingEnabled(bool enabled) { materialData_->enableLighting = enabled ? 1 : 0; }

	///
	/// Specular
	/// 
	bool IsSpecularEnabled() const { return materialData_->enableSpecular; }
	void SetMaterialSpecularEnabled(bool enable) { materialData_->enableSpecular = enable; }

	///
	/// HalfVector
	/// 
	bool IsHalfVectorEnabled() const { return materialData_->isHalfVector; }
	void SetMaterialHalfVectorEnabled(bool enable) { materialData_->isHalfVector = enable; }

	///
	/// Shininess
	///
	float GetMaterialShininess() const { return materialData_->shininess; }
	void SetMaterialShininess(float shininess) { materialData_->shininess = shininess; }

	///
	/// UV Transform
	/// 
	const Matrix4x4& GetMaterialUVTransform() const { return materialData_->uvTransform; }
	void SetMaterialUVTransform(const Matrix4x4& uvTransform) { materialData_->uvTransform = uvTransform; }

	///
	/// Material Enable
	/// 
	bool IsMaterialEnabled() const { return materialData_->enableLighting != 0; }
	void SetMaterialEnabled(bool enable) { materialData_->enableLighting = enable; }

	///
	/// Resource
	/// 
	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	MaterialData* GetMaterialData() { return materialData_; }



private:
	/*=================================================================
	
								ポインタ

	=================================================================*/
	DirectXCommon* dxCommon_ = nullptr;





	/*=================================================================

								リソース作成

	=================================================================*/

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	MaterialData* materialData_ = nullptr;


};

