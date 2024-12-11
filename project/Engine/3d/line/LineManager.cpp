#include "LineManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

// シングルトンインスタンスの初期化
std::unique_ptr<LineManager> LineManager::instance = nullptr;
std::once_flag LineManager::initInstanceFlag;

LineManager* LineManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance.reset(new LineManager());
		});
	return instance.get();
}

void LineManager::Initialize()
{
	// ポインタを渡す
	dxCommon_ = DirectXCommon::GetInstance();
	srvManager_ = SrvManager::GetInstance();

	// パイプライン生成
	CreateGraphicsPipeline();
}

void LineManager::SetCommandListConfig()
{
	SetRootSignature();

	SetGraphicsPipeline();

	SetPrimitiveTopology();
}

void LineManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// 1. RootSignatureの作成

	descriptionRootSignature_.Flags =D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 		// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;								// レジスタ番号0とバインド
	descriptionRootSignature_.pParameters = rootParameters_;						// ルートパラメーター配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);			// 配列の長さ


	// 1. パーティクルのRootSignatureの作成
	descriptorRangeForInstancing_[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing_[0].NumDescriptors = 1;
	descriptorRangeForInstancing_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// Samplerの設定
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;						// バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;				// 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
	staticSamplers_[0].ShaderRegister = 0;												// レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// PixelShaderで使う
	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	// 2. InputLayoutの設定
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

	// 3. BlendDtateの設定
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterrizerStateの設定
	rasterrizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	rasterrizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	// 4. Shaderをコンパイルする
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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};//graphicsPipelineState_
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();					 // Rootsignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;					 // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };										 // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };											 // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;							 // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc_;   				 // RasterrizerState
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。線
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

void LineManager::SetGraphicsPipeline()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
}

void LineManager::SetRootSignature()
{
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
}

void LineManager::SetPrimitiveTopology()
{
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}
