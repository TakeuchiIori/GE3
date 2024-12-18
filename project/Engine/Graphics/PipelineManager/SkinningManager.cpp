#include "SkinningManager.h"
#include "PipelineManager/PipelineManager.h"
// シングルトンインスタンスの初期化
std::unique_ptr<SkinningManager> SkinningManager::instance = nullptr;
std::once_flag SkinningManager::initInstanceFlag;

/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
SkinningManager* SkinningManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance.reset(new SkinningManager());
		});
	return instance.get();
}

void SkinningManager::Initialize(DirectXCommon* dxCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;

	rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("Animation");
	graphicsPipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("Animation");
	// パイプラインの生成
	//CreateGraphicsPipeline();
}

//void SkinningManager::CreateRootSignature()
//{
//	HRESULT hr;
//
//	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
//	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
//	descriptorRange[0].NumDescriptors = 1; // 数は1つ
//	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
//	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
//
//
//	D3D12_DESCRIPTOR_RANGE descriptorRangeBone[1] = {};
//	descriptorRangeBone[0].BaseShaderRegister = 0;
//	descriptorRangeBone[0].NumDescriptors = 1;
//	descriptorRangeBone[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRangeBone[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//
//	//=================== RootParameter 複数設定できるので配列 ===================//
//
//	D3D12_ROOT_PARAMETER rootParameters[8] = {};
//	// マテリアル
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド
//
//	// 座標変換行列
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
//	rootParameters[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド
//
//	// テクスチャ用ディスクリプターテーブル
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数
//
//	// 平行光源
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う
//
//	// 鏡面反射
//	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
//	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[4].Descriptor.ShaderRegister = 2;									// レジスタ番号2を使う
//
//
//	// ポイントライト
//	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
//	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[5].Descriptor.ShaderRegister = 3;									// レジスタ番号3を使う
//
//	// スポットライト
//	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
//	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	rootParameters[6].Descriptor.ShaderRegister = 4;									// レジスタ番号4を使う
//
//	// アニメーション
//	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // ストラクチャーバッファー
//	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//	rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRangeBone;
//	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeBone);
//
//
//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
//	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	descriptionRootSignature.pParameters = rootParameters;								// ルートパラメーター配列へのポインタ
//	descriptionRootSignature.NumParameters = _countof(rootParameters);					// 配列の長さ
//
//
//	//=================== Samplerの設定 ===================//
//	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
//	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;							// バイリニアフィルタ
//	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
//	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;						// 比較しない
//	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
//	staticSamplers[0].ShaderRegister = 0;												// レジスタ番号0を使う
//	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
//	descriptionRootSignature.pStaticSamplers = staticSamplers;
//	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
//
//	// シリアライズしてバイナリにする
//	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
//	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
//		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
//	if (FAILED(hr)) {
//		DirectXCommon::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
//		assert(false);
//	}
//	// バイナリを元に生成
//
//	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
//		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
//	assert(SUCCEEDED(hr));
//
//	//=================== BlendDtateの設定 ===================//
//
//	// 全ての色要素を書き込む
//	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	// RasterrizerStateの設定
//	// 裏面（時計回り）を表示しない  [カリング]
//	rasterrizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
//	// 三角形の中を塗りつぶす
//	rasterrizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
//
//	//=================== Shaderをコンパイルする ===================//
//
//
//	vertexShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Skinning.VS.hlsl",
//		L"vs_6_0");
//	assert(vertexShaderBlob_ != nullptr);
//	pixelShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Skinning.PS.hlsl",
//		L"ps_6_0");
//	assert(pixelShaderBlob_ != nullptr);
//
//	//=================== DepthStencilStateの設定 ===================//
//	// Depthの機能を有効化する
//	depthStencilDesc_.DepthEnable = true;
//	//書き込みします
//	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//	//比較関数はLessEqual。つまり、近ければ描画される
//	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//}
//
//void SkinningManager::CreateGraphicsPipeline()
//{
//	CreateRootSignature();
//
//	//=================== InputLayoutの設定 ===================//
//	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
//	inputElementDescs[0].SemanticName = "POSITION";
//	inputElementDescs[0].SemanticIndex = 0;
//	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//	inputElementDescs[1].SemanticName = "TEXCOORD";
//	inputElementDescs[1].SemanticIndex = 0;
//	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
//	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//	inputElementDescs[2].SemanticName = "NORMAL";
//	inputElementDescs[2].SemanticIndex = 0;
//	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//	inputElementDescs[3].SemanticName = "WEIGHT";
//	inputElementDescs[3].SemanticIndex = 0;
//	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	inputElementDescs[3].InputSlot = 1;
//	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//	inputElementDescs[4].SemanticName = "INDEX";
//	inputElementDescs[4].SemanticIndex = 0;
//	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
//	inputElementDescs[4].InputSlot = 1;
//	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//
//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
//	inputLayoutDesc.pInputElementDescs = inputElementDescs;
//	inputLayoutDesc.NumElements = _countof(inputElementDescs);
//
//
//	HRESULT hr;
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
//	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();			
//	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;					
//	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
//	vertexShaderBlob_->GetBufferSize() };										
//	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
//	pixelShaderBlob_->GetBufferSize() };										
//	graphicsPipelineStateDesc.BlendState = blendDesc_;							
//	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc_;   			
//	// Depthstencitの設定
//	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
//	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	// 書き込むRTVの情報
//	graphicsPipelineStateDesc.NumRenderTargets = 1;
//	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//	// 利用するトポロジ（形状）のタイプ。三角形
//	graphicsPipelineStateDesc.PrimitiveTopologyType =
//		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
//	graphicsPipelineStateDesc.SampleDesc.Count = 1;
//	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//	// 実際に生成
//
//	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&graphicsPipelineState_));
//	assert(SUCCEEDED(hr));
//
//}

void SkinningManager::DrawPreference()
{
	// ルートシグネチャをセット
	SetRootSignature();

	// パイプラインをセット
	SetGraphicsCommand();

	// プリミティブトポロジーをセット
	SetPrimitiveTopology();
}

void SkinningManager::SetRootSignature()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
}

void SkinningManager::SetGraphicsCommand()
{
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
}

void SkinningManager::SetPrimitiveTopology()
{
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
