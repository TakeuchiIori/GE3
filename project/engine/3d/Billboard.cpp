#include "Billboard.h"
#include "Vector4.h"
void Billboard::Initialize(DirectXCommon* dxCommon, const std::string& textureFilePath)
{
	dxCommon_ = dxCommon;
	textureFilePath_ = textureFilePath;

	// 頂点バッファ、マテリアル、グラフィックスパイプラインの初期化
	CreateVertexResource();
	CreateMaterialResource();
	CreateGraphicsPipeline();
}

void Billboard::Draw(const Camera& camera)
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Matrix4x4 viewMatrix = Inverse(MakeAffineMatrix(camera.GetScale(), camera.GetRotate(), camera.GetTranslate()));
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

	for (const auto& point : railPoints_) {
		Matrix4x4 worldMatrix = MakeTranslationMatrix(point);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

		// WVP行列をシェーダーに送信
		dxCommon_->GetCommandList()->SetGraphicsRoot32BitConstants(0, 16, &worldViewProjectionMatrix, 0);

		// 板ポリの描画
		dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	}
}

void Billboard::SetRailPoints(const std::vector<Vector3>& points)
{
	railPoints_ = points;
}

void Billboard::CreateVertexResource()
{
	// 板ポリ用の四角形を定義
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
	};
	std::vector<VertexData> vertices = {
		{ { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
	};

	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertices.size();
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	vertexResource_->Unmap(0, nullptr);
}

void Billboard::CreateRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	D3D12_ROOT_PARAMETER rootParameters[1] = {};

	// WVP行列の定数バッファ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[0].Constants.Num32BitValues = 16;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[0].Constants.ShaderRegister = 0;

	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	assert(SUCCEEDED(hr));

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void Billboard::CreateGraphicsPipeline()
{
	CreateRootSignature();

	// 入力レイアウトの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);

	// シェーダーのロード
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Particle.VS.hlsl", L"vs_6_0");
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Particle.PS.hlsl", L"ps_6_0");
	assert(vertexShaderBlob != nullptr);
	assert(pixelShaderBlob != nullptr);

	// グラフィックスパイプラインステートの作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = rootSignature_.Get();
	pipelineStateDesc.InputLayout = inputLayoutDesc_;
	pipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
	pipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.SampleDesc.Count = 1;

	// ブレンドステートの設定
	pipelineStateDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
	pipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	pipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	// ラスタライザーステートの設定
	pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// デプスステンシルの設定
	pipelineStateDesc.DepthStencilState.DepthEnable = true;
	pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void Billboard::CreateMaterialResource()
{
	// マテリアルリソースを作成
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Vector4));
	Vector4* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialResource_->Unmap(0, nullptr);
}
