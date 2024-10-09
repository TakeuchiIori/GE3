#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <numbers>
#include "Vector3.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

ParticleManager* ParticleManager::instance = nullptr;
// ドロップダウンメニュー用の文字列
const char* blendModeNames[] = {
	"None",
	"Normal",
	"Add",
	"Subtract",
	"Multiply",
	"Screen"
};
ParticleManager* ParticleManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleManager();
	}
    return instance;
}

void ParticleManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    // ポインタを渡す
    this->dxCommon_ = dxCommon;
    this->srvManager_ = srvManager;

	// パイプライン生成
	CreateGraphicsPipeline();

	// 四角形の頂点を作成
	CreateVertexResource();

	// ランダムエンジン初期化
	InitRandomEngine();
}

void ParticleManager::Update()
{

	uint32_t numInstance = 0;
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 backFrontMatirx = MakeRotateMatrixY(numbers::pi_v<float>);
	Matrix4x4 viewProjectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	// パーティクルの更新処理を行う場合
	if (particleUpdate) {
		for (auto& group : particleGroups_) {
			auto& particles = group.second.particles; // 各パーティクルグループのパーティクルリストを取得
			uint32_t numInstance = 0; // パーティクルグループごとにインスタンス数をリセット

			// パーティクルの更新
			for (auto particleIterator = particles.begin(); particleIterator != particles.end();) {
				// パーティクルの寿命をチェック
				if (particleIterator->lifeTime <= particleIterator->currentTime) {
					particleIterator = particles.erase(particleIterator);
					continue;
				}

				// 速度に加速度フィールドを適用
				if (IsCollision(accelerationField.area, particleIterator->transform.translate)) {
					particleIterator->velocity += accelerationField.acceleration * kDeltaTime;
				}

				// 現在時間を更新
				particleIterator->currentTime += kDeltaTime;

				// 次のパーティクルに進む
				++particleIterator;
			}

			// パーティクルのエミッター処理
			emitter.frequencyTime += kDeltaTime;
			if (emitter.frequency <= emitter.frequencyTime) {
				// 新しいパーティクルを発生させる
				particles.splice(particles.end(), Emit(emitter, randomEngine_));
				emitter.frequencyTime -= emitter.frequency; // 頻度をリセット
			}

			// インスタンシングデータの更新
			for (auto particleIterator = particles.begin(); particleIterator != particles.end(); ++particleIterator) {
				Matrix4x4 billboardMatrix;
				if (useBillboard) {
					// ビルボード行列の計算
					billboardMatrix = Multiply(backFrontMatirx, cameraMatrix);
					billboardMatrix.m[3][0] = 0.0f;
					billboardMatrix.m[3][1] = 0.0f;
					billboardMatrix.m[3][2] = 0.0f;
				}
				else {
					// 単位行列を設定
					billboardMatrix = MakeIdentity4x4();
				}

				// ワールド行列の作成
				Matrix4x4 worldMatrix = MakeAffineMatrix(particleIterator->transform.scale,
					particleIterator->transform.rotate,
					particleIterator->transform.translate);

				if (useBillboard) {
					worldMatrix = Multiply(worldMatrix, billboardMatrix);
				}

				// インスタンシングデータの更新
				if (numInstance < kNumMaxInstance) {
					Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

					// instancingDataはポインタとして扱われるが、配列のようにインデックスアクセスを使用する
					group.second.instancingData[numInstance].WVP = worldViewProjectionMatrix;
					group.second.instancingData[numInstance].World = worldMatrix;
					group.second.instancingData[numInstance].color = particleIterator->color;

					// パーティクルのアルファ値を計算
					float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
					group.second.instancingData[numInstance].color.w = alpha;

					// パーティクルの位置を更新
					particleIterator->transform.translate += particleIterator->velocity * kDeltaTime;

					// インスタンス数をインクリメント
					++numInstance;
				}

			}
		}
	}
}

void ParticleManager::Draw()
{
	dxCommon_->GetcommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon_->GetcommandList()->SetPipelineState(graphicsPipelineState.Get());   // PSOを設定
	dxCommon_->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);	  // VBVを設定
	dxCommon_->GetcommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// 1. RootSignatureの作成

	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるので配列。

	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う

	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// VertexShaderで使う

	rootParameters_[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う
	descriptionRootSignature_.pParameters = rootParameters_;								// ルートパラメーター配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);					// 配列の長さ


	// 1. パーティクルのRootSignatureの作成
	
	descriptorRangeForInstancing_[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing_[0].NumDescriptors = 1;
	descriptorRangeForInstancing_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing_;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing_);


	// Samplerの設定
	
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;							// バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;						// 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
	staticSamplers_[0].ShaderRegister = 0;												// レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
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
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = dxCommon_->Getdevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));


	// 2. InputLayoutの設定
	
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc.NumElements = _countof(inputElementDescs_);

	// 3. BlendDtateの設定
	
	// 全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = true;
	BlendMode blendMode{};
	SetBlendMode(blendDesc_, blendMode);
	currentBlendMode_ = kBlendModeAdd;  // 現在のブレンドモード
	// α値のブレンド
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasterrizerStateの設定
	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	// 裏面（時計回り）を表示しない  [カリング]
	rasterrizerDesc.CullMode = D3D12_CULL_MODE_NONE;/* D3D12_CULL_MODE_*/
	// 三角形の中を塗りつぶす
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// 4. Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Particle.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/Particle.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

}

void ParticleManager::CreateGraphicsPipeline()
{
	// ルートシグネチャ
	CreateRootSignature();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
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
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	HRESULT hr = dxCommon_->Getdevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

}

void ParticleManager::CreateVertexResource()
{
	// 四角形
	modelData_.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.material.textureFilePath = "Resources/images/circle.png";

	// バッファビュー作成
	CreateVertexVBV();
	// 頂点データを書き込む
	UploadVertexResource();
}

void ParticleManager::CreateVertexVBV()
{
	// 頂点バッファービュー
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();// リソースデータの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());// 使用するリソースのサイズは1頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);// 1頂点のサイズ
}

void ParticleManager::UploadVertexResource()
{
	//頂点リソース
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	//　データ書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void ParticleManager::InitRandomEngine()
{
	// ランダムエンジンの初期化
	randomEngine_ = std::mt19937(seedGenerator_());
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath)
{
	// 登録済みの名前かチェック
	assert(particleGroups_.find(name) == particleGroups_.end());
	
	// 新しい空のパーティクルグループを作成してコンテナに登録
	ParticleGroup newGroup;

	// マテリアルデータにテクスチャファイルパスを設定
	newGroup.materialData.textureFilePath = textureFilePath;
	// テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(newGroup.materialData.textureFilePath);
	// マテリアルデータにテクスチャのSRVインデックスを記録
	newGroup.materialData.textureIndexSRV = TextureManager::GetInstance()->GetTextureIndexByFilePath(newGroup.materialData.textureFilePath);
	
	// Instancing用のTransformationMatrixリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource =
		dxCommon_->CreateBufferResource( sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	ParticleForGPU* instancingData = nullptr;
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込む
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData[index].WVP = MakeIdentity4x4();
		instancingData[index].World = MakeIdentity4x4();
		instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = srvManager_->GetCPUSRVDescriptorHandle(3);
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = srvManager_->GetGPUSRVDescriptorHandle(3);

	// 3 Particle
	dxCommon_->Getdevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);


	// パーティクルグループをコンテナに登録
	particleGroups_[name] = newGroup;


}

void ParticleManager::SetBlendMode(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode)
{
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		blendDesc.RenderTarget[i].BlendEnable = TRUE;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	switch (blendMode)
	{
	case kBlendModeNormal:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	case kBlendModeAdd:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultiply:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		break;
	case kBlendModeScreen:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		// 他のモードは処理なし
		 // kBlendModeNone や kBlendModeNormal の場合はデフォルト設定を使用
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendDesc.RenderTarget[i].BlendEnable = FALSE;
		}
		break;
	}
}

void ParticleManager::ShowBlendModeDropdown(BlendMode& currentBlendMode)
{
#ifdef DEBUG

	if (ImGui::BeginCombo("Blend Mode", blendModeNames[currentBlendMode]))
	{
		for (int i = 0; i < kCount0fBlendMode; ++i)
		{
			bool isSelected = (currentBlendMode == static_cast<BlendMode>(i));
			if (ImGui::Selectable(blendModeNames[i], isSelected))
			{
				currentBlendMode = static_cast<BlendMode>(i);
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
#endif // DEBUG
}


// メインループの一部として呼び出す
void ParticleManager::Render(D3D12_BLEND_DESC& blendDesc, BlendMode& currentBlendMode)
{
	ShowBlendModeDropdown(currentBlendMode);

	// ブレンドモードの変更を適用
	SetBlendMode(blendDesc, currentBlendMode);
}