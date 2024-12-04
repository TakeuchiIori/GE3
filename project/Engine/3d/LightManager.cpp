#include "LightManager.h"
#include "DirectXCommon.h"
LightManager::LightManager()
{
}

LightManager::~LightManager()
{
    // リソースの解放処理
}

void LightManager::Initialize()
{
    this->dxCommon_ = DirectXCommon::GetInstance();

    // リソース作成関数の呼び出し
    CreateDirectionalLightResource();
    CreatePointLightResource();
    CreateSpecularReflectionResource();
}

void LightManager::SetCommandList()
{
    // 平行光源CB
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
    // 鏡面反射CB
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, specularReflectionResource_->GetGPUVirtualAddress());
    // ポイントライト
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());

}

void LightManager::CreateDirectionalLightResource()
{
    directionalLightResource_ =dxCommon_->CreateBufferResource(sizeof(DirectionalLight));
    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
    directionalLight_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLight_->direction = { 0.0f, -1.0f, 0.0f };
    directionalLight_->intensity = 1.0f;
    directionalLight_->enableDirectionalLight = true;
}

void LightManager::CreatePointLightResource()
{
    pointLightResource_ =dxCommon_->CreateBufferResource(sizeof(PointLight));
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
    specularReflectionResource_ =dxCommon_->CreateBufferResource(sizeof(CameraForGPU));
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
