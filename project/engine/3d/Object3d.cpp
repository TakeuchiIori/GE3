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
#include <algorithm>
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

	// ポイントライト
	PointLightResource();


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
	// ポイントライト
	object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());

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
	directionalLight_->enableDirectionalLight = true;

}

void Object3d::SpecularReflectionResource()
{
	specularReflectionResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
	specularReflectionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->worldPosition = { 0.0f,0.0f,0.0f };
	cameraData_->enableSpecular = false;
	cameraData_->isHalfVector = true;
}

void Object3d::PointLightResource()
{
	pointLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLight_));

	pointLight_->position = { 0.0f,0.0f,0.0f };
	pointLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLight_->intensity = 0.0f;
	pointLight_->enableDirectionalLight = true;
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

void Object3d::ShowLightingEditor()
{
    if (ImGui::Begin("Lighting Editor")) {
        // 平行光源
        ImGui::Text("Directional Light");
        Vector3 lightDirection = GetLightDirection();
        if (ImGui::SliderFloat3("Direction", &lightDirection.x, -1.0f, 1.0f, "%.2f")) {
            SetLightDirection(lightDirection);
        }

        Vector4 lightColor = GetLightColor();
        if (ImGui::ColorEdit4("Color", &lightColor.x)) {
            SetLightColor(lightColor);
        }

        float lightIntensity = GetLightIntensity();
        if (ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 10.0f, "%.2f")) {
            SetLightIntensity(lightIntensity);
        }

        // ポイントライト
        ImGui::Separator(); // 区切り線
        ImGui::Text("Point Light");
        bool pointLightEnabled = IsPointLightEnabled();
        if (ImGui::Checkbox("Enabled", &pointLightEnabled)) {
            SetPointLightEnabled(pointLightEnabled);
        }

        Vector4 pointLightColor = GetPointLightColor();
        if (ImGui::ColorEdit4("Point Color", &pointLightColor.x)) {
            SetPointLightColor(pointLightColor);
        }

        Vector3 pointLightPosition = GetPointLightPosition();
        if (ImGui::SliderFloat3("Position", &pointLightPosition.x, -100.0f, 100.0f, "%.2f")) {
            SetPointLightPosition(pointLightPosition);
        }

        float pointLightIntensity = GetPointLightIntensity();
        if (ImGui::SliderFloat("Point Intensity", &pointLightIntensity, 0.0f, 10.0f, "%.2f")) {
            SetPointLightIntensity(pointLightIntensity);
        }

        // 鏡面反射
        ImGui::Separator(); // 区切り線
        ImGui::Text("Specular Reflection");
        bool specularEnabled = IsSpecularEnabled();
        if (ImGui::Checkbox("Enable Specular", &specularEnabled)) {
            SetSpecularEnabled(specularEnabled);
        }

        bool isHalfVector = IsHalfVector();
        if (ImGui::Checkbox("Use Half Vector", &isHalfVector)) {
            SetHalfVector(isHalfVector);
        }

        // マテリアル
        ImGui::Separator(); // 区切り線
        ImGui::Text("Material");
        Vector4 materialColor = GetMaterialColor();
        if (ImGui::ColorEdit4("Material Color", &materialColor.x)) {
            SetMaterialColor(materialColor);
        }

        float shininess = GetMaterialShininess();
        if (ImGui::SliderFloat("Shininess", &shininess, 0.1f, 200.0f, "%.2f")) {
            SetMaterialShininess(shininess);
        }

        Matrix4x4 uvTransform = GetMaterialUVTransform();
        if (ImGui::InputFloat("UV Scale X", &uvTransform.m[0][0], 0.1f, 1.0f, "%.2f")) {
            uvTransform.m[0][0] = std::clamp(uvTransform.m[0][0], 0.1f, 10.0f);
            SetMaterialUVTransform(uvTransform);
        }
        if (ImGui::InputFloat("UV Scale Y", &uvTransform.m[1][1], 0.1f, 1.0f, "%.2f")) {
            uvTransform.m[1][1] = std::clamp(uvTransform.m[1][1], 0.1f, 10.0f);
            SetMaterialUVTransform(uvTransform);
        }
    }
    ImGui::End();
}
