#include "PipelineManager.h"
#include "DX./DirectXCommon.h"


void PipelineManager::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
}

void PipelineManager::SetRootSignature(const std::string& key)
{
    // パイプラインステートの存在確認
    if (pipelineStates_.find(key) == pipelineStates_.end()) {
        // 存在しない場合は作成
        CreateRootSignature(key);
    }

    auto commandList = dxCommon_->GetCommandList();
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

    auto commandList = dxCommon_->GetCommandList();
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
        CRS_Sprite();

    }
    else if (key == "Object") {
        // オブジェクト用の設定
        CRS_Object();

    }
    else if (key == "Line") {
        // ライン用の設定
        CRS_Line();
    }

    auto device = dxCommon_->GetDevice();

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
	//===============================================================================//
	/*								ルートシグネチャ									*/
	//===============================================================================//

	HRESULT hr;
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
	

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//=================== RootParameter ===================//
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	// マテリアル
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	// 座標変換行列
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	// テクスチャ用ディスクリプターテーブル
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

	// 平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う

	// 鏡面反射
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;									// レジスタ番号2を使う


	// ポイントライト
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 3;									// レジスタ番号3を使う

	// スポットライト
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[6].Descriptor.ShaderRegister = 4;									// レジスタ番号4を使う


	descriptionRootSignature.pParameters = rootParameters;								// ルートパラメーター配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);					// 配列の長さ


	//=================== Sampler ===================//
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;							// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;						// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
	staticSamplers[0].ShaderRegister = 0;												// レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatures_["Object"]));
	assert(SUCCEEDED(hr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//=================== InputLayoutの設定 ===================//
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//=================== BlendDtateの設定 ===================//
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// RasterrizerStateの設定
	// 裏面（時計回り）を表示しない  [カリング]
	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	rasterrizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//=================== Shaderをコンパイルする ===================//
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

	vertexShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Object3d.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	pixelShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Object3d.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//=================== DepthStencilStateの設定 ===================//
	// Depthの機能を有効化する
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatures_["Object"].Get();	 // Rootsignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;					 // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };										 // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };											 // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;							 // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;   				 // RasterrizerState
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成

	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(pipelineStates_["Particle"].GetAddressOf()));
	assert(SUCCEEDED(hr));
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
