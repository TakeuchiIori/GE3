#pragma once
#include "DirectXCommon.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include <vector>
#include <wrl.h>

class Billboard
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(DirectXCommon* dxCommon, const std::string& textureFilePath);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw(const Camera& camera);

    /// <summary>
    /// レールの位置を設定
    /// </summary>
    void SetRailPoints(const std::vector<Vector3>& points);

private:
    void CreateVertexResource();
    void CreateRootSignature();
    void CreateGraphicsPipeline();
    void CreateMaterialResource();

private:
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    std::vector<Vector3> railPoints_;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
    std::string textureFilePath_;
};

