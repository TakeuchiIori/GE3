#include "LineManager.h"
#include "DX./DirectXCommon.h"
#include "SrvManager./SrvManager.h"


LineManager* LineManager::GetInstance()
{
	static LineManager instance;
	return &instance;
}

void LineManager::Initialize()
{
	// ポインタを渡す
	dxCommon_ = DirectXCommon::GetInstance();
	srvManager_ = SrvManager::GetInstance();

	// パイプライン生成
	CreateGraphicsPipeline();
}


void LineManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// マテリアル
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;								// レジスタ番号0とバインド
	
	// 座標変換行列
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;				// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 1;								// レジスタ番号0とバインド
	
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters = rootParameters;						// ルートパラメーター配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);			// 配列の長さ

	// シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	// BlendDtateの設定
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterrizerStateの設定
	rasterrizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	rasterrizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Line.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
	pixelShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Line.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

	// DepthStencilStateの設定
	depthStencilDesc_.DepthEnable = true;
	depthStencilDesc_.StencilEnable = false;
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

void LineManager::CreateGraphicsPipeline()
{

	CreateRootSignature();

	// 2. InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();			
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;					
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };										
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };										
	graphicsPipelineStateDesc.BlendState = blendDesc_;							
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc_;   			
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}