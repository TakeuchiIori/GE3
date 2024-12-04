#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"

class DirectXCommon;
class LightManager
{
public:
    // コンストラクタとデストラクタ
    LightManager();
    ~LightManager();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// コマンドリストを積む
    /// </summary>
    void SetCommandList();

    /// <summary>
    /// ライト設定関数
    /// </summary>
    /// <param name="color"></param>
    /// <param name="direction"></param>
    /// <param name="intensity"></param>
    /// <param name="enable"></param>
    void SetDirectionalLight(const Vector4& color, const Vector3& direction, float intensity, bool enable);
    void SetPointLight(const Vector4& color, const Vector3& position, float intensity, float radius, float decay, bool enable);
    void SetSpecularReflection(bool enabled, bool isHalfVector);

    /// <summary>
    /// リソースの取得関数
    /// </summary>
    ID3D12Resource* GetDirectionalLightResource() const { return directionalLightResource_.Get(); }
    ID3D12Resource* GetPointLightResource() const { return pointLightResource_.Get(); }
    ID3D12Resource* GetSpecularReflectionResource() const { return specularReflectionResource_.Get(); }


private:

    // リソース作成関数
    void CreateDirectionalLightResource();
    void CreatePointLightResource();
    void CreateSpecularReflectionResource();

public:
    //=================================================================//
    //                           アクセッサ
    //=================================================================//
    
    // 平行光源
    const Vector4& GetDirectionalLightColor() const { return directionalLight_->color; }
    void SetDirectionalLightColor(const Vector4& color) { directionalLight_->color = color; }
    const Vector3& GetDirectionalLightDirection() const { return directionalLight_->direction; }
    void SetDirectionalLightDirection(const Vector3& direction) { directionalLight_->direction = direction; }
    float GetDirectionalLightIntensity() const { return directionalLight_->intensity; }
    void SetDirectionalLightIntensity(float intensity) { directionalLight_->intensity = intensity; }
    bool IsDirectionalLightEnabled() const { return directionalLight_->enableDirectionalLight != 0; }
    void SetDirectionalLightEnabled(bool enable) { directionalLight_->enableDirectionalLight = enable; }

    // ポイントライト
    const Vector4& GetPointLightColor() const { return pointLight_->color; }
    void SetPointLightColor(const Vector4& color) { pointLight_->color = color; }
    const Vector3& GetPointLightPosition() const { return pointLight_->position; }
    void SetPointLightPosition(const Vector3& position) { pointLight_->position = position; }
    float GetPointLightIntensity() const { return pointLight_->intensity; }
    void SetPointLightIntensity(float intensity) { pointLight_->intensity = intensity; }
    float GetPointLightRadius() const { return pointLight_->radius; }
    void SetPointLightRadius(float radius) { pointLight_->radius = radius; }
    float GetPointLightDecay() const { return pointLight_->decay; }
    void SetPointLightDecay(float decay) { pointLight_->decay = decay; }
    bool IsPointLightEnabled() const { return pointLight_->enablePointLight != 0; }
    void SetPointLightEnabled(bool enable) { pointLight_->enablePointLight = enable; }

    // 鏡面反射
    const Vector3& GetCameraWorldPosition() const { return cameraData_->worldPosition; }
    void SetCameraWorldPosition(const Vector3& position) { cameraData_->worldPosition = position; }
    bool IsSpecularEnabled() const { return cameraData_->enableSpecular; }
    void SetSpecularEnabled(bool enabled) { cameraData_->enableSpecular = enabled; }
    bool IsHalfVectorUsed() const { return cameraData_->isHalfVector != 0; }
    void SetHalfVectorUsed(bool isHalfVector) { cameraData_->isHalfVector = isHalfVector; }

private:
    // 平行光源のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
    struct DirectionalLight
    {
        Vector4 color;
        Vector3 direction;
        float intensity;
        bool enableDirectionalLight;
    };
    DirectionalLight* directionalLight_ = nullptr;

    // ポイントライトのリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
    struct PointLight
    {
        Vector4 color;
        Vector3 position;
        float intensity;
        float radius;
        float decay;
        bool enablePointLight;
    };
    PointLight* pointLight_ = nullptr;

    // 鏡面反射のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> specularReflectionResource_;
    struct CameraForGPU
    {
        Vector3 worldPosition;
        bool enableSpecular;
        bool isHalfVector;
    };
    CameraForGPU* cameraData_ = nullptr;

    DirectXCommon* dxCommon_ = nullptr;
};

