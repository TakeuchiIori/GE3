#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "ModelManager.h"
#include "Model.h"
#include "WorldTransform.h"
#include "imgui.h"

void Object3d::Initialize()
{
	// 引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = Object3dCommon::GetInstance();
	// デフォルトカメラのセット
	this->camera_ = object3dCommon_->GetDefaultCamera();


	// マテリアル
	MaterialResource();

	// 平行光源
	DirectionalLightResource();

	//　鏡面反射
	SpecularReflectionResource();


}

void Object3d::Draw(WorldTransform& worldTransform)
{

	Matrix4x4 worldViewProjectionMatrix;
	if (camera_) {
		cameraData_->worldPosition = camera_->GetTranslate();
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldTransform.GetMatWorld() * viewProjectionMatrix;
	}
	else {
		worldViewProjectionMatrix = worldTransform.GetMatWorld();
	}
	worldTransform.SetMapWVP(worldViewProjectionMatrix);

	// マテリアルCB場所
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformatonMatrixCB
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.GetConstBuffer()->GetGPUVirtualAddress());
	// 平行光源CB
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 鏡面反射CB
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, specularReflectionResource_->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3d::ChangeTexture(std::string textureFilePath)
{

}

void Object3d::DirectionalLightResource()
{
	directionalLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));

	// デフォルト値を設定
	directionalLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->direction = { 0.0f,-1.0f,0.0f };
	directionalLight_->intensity = 1.0f;

}

void Object3d::SpecularReflectionResource()
{
	specularReflectionResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
	specularReflectionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = { 0.0f,0.0f,0.0f };
	cameraData_->enableSpecular = false;
	cameraData_->isHalfVector = true;
}

void Object3d::MaterialResource()
{
	// リソース作成
	materialResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;
	materialData_->shininess = 1.0f;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

