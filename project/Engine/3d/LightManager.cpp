#include "LightManager.h"
#include "DirectXCommon.h"
#include "Object3dCommon.h"
#include <algorithm>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


LightManager* LightManager::GetInstance()
{
    static LightManager instance;
    return &instance;
}

void LightManager::Initialize()
{
  
    this->object3dCommon_ = Object3dCommon::GetInstance();
    // デフォルトカメラのセット
    this->camera_ = object3dCommon_->GetDefaultCamera();
    // リソース作成関数の呼び出し
    CreateDirectionalLightResource();
    CreatePointLightResource();
    CreateSpecularReflectionResource();

}

void LightManager::SetCommandList()
{
    if (camera_) {
        cameraData_->worldPosition = camera_->GetTranslate();
    }
    // 平行光源CB
    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
    // 鏡面反射CB
    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, specularReflectionResource_->GetGPUVirtualAddress());
    // ポイントライト
    object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());

}



void LightManager::CreateDirectionalLightResource()
{
    directionalLightResource_ =object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
    directionalLight_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLight_->direction = { 0.0f, -1.0f, 0.0f };
    directionalLight_->intensity = 0.0f;
    directionalLight_->enableDirectionalLight = false;
}

void LightManager::CreatePointLightResource()
{
    pointLightResource_ =object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(PointLight));
    pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLight_));
    pointLight_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    pointLight_->position = { 0.0f, 2.0f, 0.0f };
    pointLight_->intensity = 1.0f;
    pointLight_->radius = 10.0f;
    pointLight_->decay = 1.0f;
    pointLight_->enablePointLight = true;
}

void LightManager::CreateSpecularReflectionResource()
{
    specularReflectionResource_ =object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
    specularReflectionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
    cameraData_->worldPosition = { 0.0f, 0.0f, 0.0f };
    cameraData_->enableSpecular = false;
    cameraData_->isHalfVector = false;
}

void LightManager::SetDirectionalLight(const Vector4& color, const Vector3& direction, float intensity, bool enable)
{
    directionalLight_->color = color;
    directionalLight_->direction = direction;
    directionalLight_->intensity = intensity;
    directionalLight_->enableDirectionalLight = enable;
}

void LightManager::SetPointLight(const Vector4& color, const Vector3& position, float intensity, float radius, float decay, bool enable)
{
    pointLight_->color = color;
    pointLight_->position = position;
    pointLight_->intensity = intensity;
    pointLight_->radius = radius;
    pointLight_->decay = decay;
    pointLight_->enablePointLight = enable;
}

void LightManager::SetSpecularReflection(bool enabled, bool isHalfVector)
{
    cameraData_->enableSpecular = enabled;
    cameraData_->isHalfVector = isHalfVector;
}
void LightManager::ShowLightingEditor()
{
    if (ImGui::Begin("Lighting Editor")) {
        // 平行光源
        ImGui::Text("Directional Light");
        bool directionalLightEnabled = IsDirectionalLightEnabled();
        if (ImGui::Checkbox("Directional Enabled", &directionalLightEnabled)) {
            SetDirectionalLightEnabled(directionalLightEnabled);
        }

        Vector3 lightDirection = GetDirectionalLightDirection();
        if (ImGui::SliderFloat3("Direction", &lightDirection.x, -1.0f, 1.0f, "%.2f")) {
            SetDirectionalLightDirection(lightDirection);
        }

        Vector4 lightColor = GetDirectionalLightColor();
        if (ImGui::ColorEdit4("Color", &lightColor.x)) {
            SetDirectionalLightColor(lightColor);
        }

        float lightIntensity = GetDirectionalLightIntensity();
        if (ImGui::SliderFloat("Intensity", &lightIntensity, 0.0f, 10.0f, "%.2f")) {
            SetDirectionalLightIntensity(lightIntensity);
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
        if (ImGui::SliderFloat3("Position", &pointLightPosition.x, -10.0f, 10.0f, "%.2f")) {
            SetPointLightPosition(pointLightPosition);
        }

        float pointLightIntensity = GetPointLightIntensity();
        if (ImGui::SliderFloat("Point Intensity", &pointLightIntensity, 0.0f, 10.0f, "%.2f")) {
            SetPointLightIntensity(pointLightIntensity);
        }

        float radius = GetPointLightRadius();
        if (ImGui::SliderFloat("Point Radius", &radius, 0.0f, 10.0f, "%.2f")) {
            SetPointLightRadius(radius);
        }

        float decay = GetPointLightDecay();
        if (ImGui::SliderFloat("Point Decay", &decay, 0.0f, 10.0f, "%.2f")) {
            SetPointLightDecay(decay);
        }

        // 鏡面反射
        ImGui::Separator(); // 区切り線
        ImGui::Text("Specular Reflection");
        bool specularEnabled = IsSpecularEnabled();
        if (ImGui::Checkbox("Enable Specular", &specularEnabled)) {
            SetSpecularEnabled(specularEnabled);
        }

        bool isHalfVector = IsHalfVectorUsed();
        if (ImGui::Checkbox("Use Half Vector", &isHalfVector)) {
            SetHalfVectorUsed(isHalfVector);
        }
    }
    ImGui::End();
}
