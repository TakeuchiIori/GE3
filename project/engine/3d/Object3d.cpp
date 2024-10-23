#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "ModelManager.h"
#include "Model.h"
#include "WorldTransform.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = object3dCommon;

	// デフォルトカメラのセット
	this->camera_ = object3dCommon_->GetDefaultCamera();


	// 平行光源の初期化
	DirectionalLightResource();

	// 座標変換行列の初期化
	//TransformationInitialize();

	// マテリアルリソース
	MaterialResource();
	

}

void Object3d::Draw(WorldTransform& worldTransform)
{
	

	Matrix4x4 worldViewProjectionMatrix;
	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldTransform.GetMatWorld() * viewProjectionMatrix;
	}
	else {
		worldViewProjectionMatrix = worldTransform.GetMatWorld();
	}
	worldTransform.SetMapWVP(worldViewProjectionMatrix);

	// マテリアルCBufferの場所を指定
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformatonMatrixCBuffferの場所を設定
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.GetConstBuffer()->GetGPUVirtualAddress());
	// 平行光源のCBufferの場所を設定
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3d::ChangeTexture(std::string textureFilePath)
{

}


//void Object3d::TransformationInitialize()
//{
//	////	TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
//	//transformationMatrixResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
//	//// データを書き込むためのアドレスを取得して割り当て
//	//transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
//	//// 単位行列を書き込む
//	//transformationMatrixData_->WVP = MakeIdentity4x4();
//	//transformationMatrixData_->World = MakeIdentity4x4();
//}

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

void Object3d::MaterialResource()
{
	// リソース作成
	materialResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f};
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Object3d::SetModel(const std::string& filePath)
{
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

