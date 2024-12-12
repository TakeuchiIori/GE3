#include "PipelineManager.h"
#include "DX./DirectXCommon.h"


void PipelineManager::Initialize()
{
}

void PipelineManager::SetRootSignature(const std::string& key)
{
    // パイプラインステートの存在確認
    if (pipelineStates_.find(key) == pipelineStates_.end()) {
        // 存在しない場合は作成
        CreateRootSignature(key);
    }

    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    //ルートシグネチャをコマンドリストに設定
    commandList->SetGraphicsRootSignature(rootSignatures_[key].Get());
}

void PipelineManager::SetPipeline(const std::string& key)
{
    // パイプラインステートの存在確認
    if (pipelineStates_.find(key) == pipelineStates_.end()) {
        // 存在しない場合は作成
        CreatePipelineState(key);
    }

    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    // パイプラインステートをコマンドリストに設定
    commandList->SetPipelineState(pipelineStates_[key].Get());
}

void PipelineManager::CreateRootSignature(const std::string& key)
{
}

void PipelineManager::CreatePipelineState(const std::string& key)
{
    // パイプラインステートの設定を構築
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    if (key == "Sprite") {
        // スプライト用の設定


    }
    else if (key == "Object") {
        // オブジェクト用の設定


    }
    else if (key == "Line") {
        // ライン用の設定


    }

    auto device = DirectXCommon::GetInstance()->GetDevice();

    // ルートシグネチャの作成または取得
    if (rootSignatures_.find(key) == rootSignatures_.end()) {
        // ルートシグネチャの作成処理
        D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
        // ... 設定
        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to serialize root signature");
        }

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create root signature");
        }
        rootSignatures_[key] = rootSignature;
    }

    // パイプラインステートにルートシグネチャを設定
    desc.pRootSignature = rootSignatures_[key].Get();

    // パイプラインステートの作成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create graphics pipeline state");
    }

    pipelineStates_[key] = pipelineState;
}

void PipelineManager::CRS_Sprite()
{

}

void PipelineManager::CRS_Object()
{
}

void PipelineManager::CRS_Line()
{
}

void PipelineManager::CGP_Sprite()
{
}

void PipelineManager::CGP_Object()
{
}

void PipelineManager::CGP_Line()
{
}
