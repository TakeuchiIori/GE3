#include <Windows.h>
#include <string>
#include <format>
#include <cstdint>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "VertexData.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "Transform.h"

#include "externals/imgui/imgui_impl_dx12.h"

#include "externals/DirectXTex/DirectXTex.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"


#include "DescriptorHandle.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "math/MathFunc.h"
#include "TransformationMatrix.h"
#include "LoadObjFile.h"
#include <corecrt_math_defines.h>

#include<wrl.h>
#include "ResourceObject.h"

#include "D3DResorceLeakChecker.h"
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "externals/imgui/imgui_impl_win32.h"


#pragma comment(lib,"dxguid.lib")

#pragma comment(lib,"dxcompiler.lib")

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResorceLeakChecker* leakCheack;
	WinApp* winApp_ = nullptr;
	winApp_ = new WinApp();
	winApp_->Initialize();

	
	Input* input_ = nullptr;
	input_ = new Input();
	input_->Initialize(winApp_);

	DirectXCommon* dxCommon = nullptr;
	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp_);

#pragma region 基礎システムの初期化

	SpriteCommon* spriteCommon = nullptr;
	// スプライト共通部の初期化
	spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

#pragma endregion 基礎システムの初期化

#pragma region 最初のシーンの初期化

	Sprite* sprite = nullptr;
	// スプライト共通部の初期化
	sprite = new Sprite();
	sprite->Initialize();

#pragma endregion 最初のシーンの終了

HRESULT hr;
	//------------------ Pipeline State Object ------------------//
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
	// 1. RootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// VertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う
	descriptionRootSignature.pParameters = rootParameters;								// ルートパラメーター配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);					// 配列の長さ
	// Samplerの設定
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
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = dxCommon->Getdevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));


	// 2. InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
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

	// 3. BlendDtateの設定
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// RasterrizerStateの設定
	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	// 裏面（時計回り）を表示しない  [カリング]
	rasterrizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// 4. Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"Resources/shaders/Object3d.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"Resources/shaders/Object3d.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//------------------ Pipeline State Objectを生成 ------------------//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();					 // Rootsignature
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
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = dxCommon->Getdevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	

	//------------------ Material用のリソース ------------------//
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = dxCommon->CreateBufferResource(sizeof(Material));
	// マテリアルのデータを書き込む
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は赤を書き込んでみる	// 03_00で白に変更
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// WVP用のリソースを作る。　1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込んでおく
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
	// SphereはLightingするのでtrue
	//materialData->enableLighting = true;

	//------------------ Sprite用のリソース ------------------//
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = dxCommon->CreateBufferResource(sizeof(Material));
	// マテリアルのデータを書き込む
	Material* materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	// 今回は赤を書き込んでみる	// 03_00で白に変更
	materialDataSprite->color = { 0.0f,0.0f, 0.0f, 0.0f };
	// WVP用のリソースを作る。TransformationMatrix　1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResourceSprite = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* wvpDataSprite = nullptr;
	// 書き込むかめのアドレスを取得
	wvpResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite));
	// 単位行列を書き込んでおく
	wvpDataSprite->WVP = MakeIdentity4x4();
	wvpDataSprite->World = MakeIdentity4x4();
	// SpriteはLightingしないのでfalseにする
	materialDataSprite->enableLighting = false;

	//------------------ 平行光源用のリソース ------------------//
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectionalLight));
	// データを書き込む
	DirectionalLight* directionalLight = nullptr;
	// 書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight));
	// デフォルト値
	directionalLight->color = { 0.0f,0.0f,0.0f,0.0f };
	directionalLight->direction = { 0.0f,-1.0f,0.0f };
	directionalLight->intensity = 1.0f;
	//// WVP用のリソースを作る。Matrix4x4　1つ分のサイズを用意する
	//ID3D12Resource* wvpResourceDirectional = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));
	//// データを書き込む
	//TransformationMatrix* wvpDataDirectional = nullptr;
	//// 書き込むためのアドレスを取得
	//wvpResourceDirectional->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataDirectional));
	//// 単位行列を書き込んでおく
	//wvpDataDirectional->WVP = MakeIdentity4x4();
	//wvpDataDirectional->World = MakeIdentity4x4();

	//------------------ VertexResourceを生成する ------------------//
	const uint32_t kSubdivision = 512;
	float vertexSphere = kSubdivision * kSubdivision * 6;
	// 詳しいことは関数を参照
	//ID3D12Resource* vertexResource =dxCommon-> CreateBufferResource(sizeof(VertexData) * vertexSphere);
	// SphereのIndex用のリソース
	//ID3D12Resource* indexResourceSphere = dxCommon->CreateBufferResource(sizeof(uint32_t) * vertexSphere);


	// Sprite用の頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);
	
	// SpriteのIndex用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	//------------------ VertexBufferViewを設定する ------------------//
	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//// リソースの先頭アドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//// 使用するリソースサイズは頂点3つ分のサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexSphere;
	//// 1頂点あたりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);
	//------------------ SpriteのVertexBufferViewを設定する ------------------//

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭アドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	//------------------ SphereのIndexBufferViewを設定する ------------------//
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};
	//// リソースの先頭アドレスから使う
	//indexBufferViewSphere.BufferLocation = indexResourceSphere->GetGPUVirtualAddress();
	//// 使用するリソースサイズは頂点6つ分のサイズ
	//indexBufferViewSphere.SizeInBytes = sizeof(uint32_t) * vertexSphere;
	//// インデックスはuint32_tとする
	//indexBufferViewSphere.Format = DXGI_FORMAT_R32_UINT;
	//------------------ SpriteのIndexBufferViewを設定する ------------------//
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	// リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースサイズはもとの頂点のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	ModelData modelData = LoadObjFile("Resources", "axis.obj");
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.verteces.size());
	// 頂点バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();// リソースデータの先頭アドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.verteces.size());// 使用するリソースのサイズは1頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);// 1頂点のサイズ

	//　データ書き込み
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//// 左下
	//vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	//vertexData[0].texcoord = { 0.0f,1.0f };
	//// 上
	//vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	//vertexData[1].texcoord = { 0.5f,0.0f };
	//// 右下
	//vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };

	//// 左下2
	//vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	//vertexData[3].texcoord = { 0.0f,1.0f };
	//// 上2
	//vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	//vertexData[4].texcoord = { 0.5f,0.0f };
	//// 右下2
	//vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };
	memcpy(vertexData, modelData.verteces.data(), sizeof(VertexData)* modelData.verteces.size());
	//------------------ 頂点リソースにデータを書き込む ------------------//
	//VertexData* vertexData = nullptr;
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//uint32_t lonindex = 0;
	//uint32_t latindex = 0;

	//// 経度分割1つ分の角度。φd
	//const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	//// 緯度分割1つ分の角度。θd
	//const float kLatEvery = float(M_PI) / float(kSubdivision);
	//// 緯度の方向に分割
	//for (latindex = 0; latindex < kSubdivision; ++latindex) {
	//	float lat = float(-M_PI) / 2.0f + kLatEvery * latindex;// θ
	//	// 経度の方向に分割しながら線を引く
	//	for (lonindex = 0; lonindex < kSubdivision; ++lonindex) {
	//		uint32_t start = (latindex * kSubdivision + lonindex) * 6;
	//		float lon = lonindex * kLonEvery; // φ
	//		float u = float(lonindex) / float(kSubdivision);
	//		float v = 1.0f - float(latindex) / float(kSubdivision);
	//		// 頂点にデータを入力する
	//		vertexData[start].position = { cos(lat) * cos(lon),sin(lat),cos(lat) * sin(lon),1.0f };
	//		vertexData[start].texcoord = { u,v };
	//		vertexData[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
	//		vertexData[start + 1].texcoord = { u,v - 1.0f / float(kSubdivision) };
	//		vertexData[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
	//		vertexData[start + 2].texcoord = { u + 1.0f / float(kSubdivision),v };
	//		vertexData[start + 3].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
	//		vertexData[start + 3].texcoord = { u + 1.0f / float(kSubdivision),v - 1.0f / float(kSubdivision) };
	//		vertexData[start + 4].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
	//		vertexData[start + 4].texcoord = { u + 1.0f / float(kSubdivision),v };
	//		vertexData[start + 5].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
	//		vertexData[start + 5].texcoord = { u,v - 1.0f / float(kSubdivision) };
	//	}
	//}
	// 球の計算
	//uint32_t lonindex = 0;
	//uint32_t latindex = 0;

	//// 経度分割1つ分の角度。φd
	//const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	//// 緯度分割1つ分の角度。θd
	//const float kLatEvery = float(M_PI) / float(kSubdivision);
	//// 緯度の方向に分割
	//for (latindex = 0; latindex < kSubdivision; ++latindex) {
	//	float lat = float(-M_PI) / 2.0f + kLatEvery * latindex;// θ
	//	// 経度の方向に分割しながら線を引く
	//	for (lonindex = 0; lonindex < kSubdivision; ++lonindex) {
	//		uint32_t start = (latindex * kSubdivision + lonindex) * 6;
	//		float lon = lonindex * kLonEvery; // φ
	//		//uint32_t startIndex = (latindex * kSubdivision + lonindex) * 6;
	//		// 頂点にデータを入力する
	//		// a頂点
	//		vertexData[start].position.x = cos(lat) * cos(lon);
	//		vertexData[start].position.y = sin(lat);
	//		vertexData[start].position.z = cos(lat) * sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { float(lonindex) / float(kSubdivision) , 1.0f - float(latindex) / float(kSubdivision) };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		// b頂点
	//		vertexData[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
	//		vertexData[start + 1].position.y = sin(lat + kLatEvery);
	//		vertexData[start + 1].position.z = cos(lat + kLatEvery) * sin(lon),
	//			vertexData[start + 1].position.w = 1.0f;
	//		vertexData[start + 1].texcoord = { float(lonindex) / float(kSubdivision),  1.0f - float(latindex + 1) / float(kSubdivision) };
	//		vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
	//		vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
	//		vertexData[start + 1].normal.z = vertexData[start + 1].position.z;
	//		// c頂点
	//		vertexData[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
	//		vertexData[start + 2].position.y = sin(lat);
	//		vertexData[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
	//		vertexData[start + 2].position.w = 1.0f;
	//		vertexData[start + 2].texcoord = { float(lonindex + 1) / float(kSubdivision),1.0f - float(latindex) / float(kSubdivision) };
	//		vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
	//		vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
	//		vertexData[start + 2].normal.z = vertexData[start + 2].position.z;
	//		// d頂点
	//		vertexData[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
	//		vertexData[start + 3].position.y = sin(lat + kLatEvery);
	//		vertexData[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
	//		vertexData[start + 3].position.w = 1.0f;
	//		vertexData[start + 3].texcoord = { float(lonindex + 1) / float(kSubdivision),1.0f - float(latindex + 1) / float(kSubdivision) };
	//		vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
	//		vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
	//		vertexData[start + 3].normal.z = vertexData[start + 3].position.z;
	//		// c頂点
	//		vertexData[start + 4].position.x = cos(lat) * cos(lon + kLonEvery);
	//		vertexData[start + 4].position.y = sin(lat);
	//		vertexData[start + 4].position.z = cos(lat) * sin(lon + kLonEvery);
	//		vertexData[start + 4].position.w = 1.0f;
	//		vertexData[start + 4].texcoord = { float(lonindex + 1) / float(kSubdivision),1.0f - float(latindex) / float(kSubdivision) };
	//		vertexData[start + 4].normal.x = vertexData[start + 4].position.x;
	//		vertexData[start + 4].normal.y = vertexData[start + 4].position.y;
	//		vertexData[start + 4].normal.z = vertexData[start + 4].position.z;
	//		// b頂点
	//		vertexData[start + 5].position.x = cos(lat + kLatEvery) * cos(lon);
	//		vertexData[start + 5].position.y = sin(lat + kLatEvery);
	//		vertexData[start + 5].position.z = cos(lat + kLatEvery) * sin(lon),
	//			vertexData[start + 5].position.w = 1.0f;
	//		vertexData[start + 5].texcoord = { float(lonindex) / float(kSubdivision),  1.0f - float(latindex + 1) / float(kSubdivision) };
	//		vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
	//		vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
	//		vertexData[start + 5].normal.z = vertexData[start + 5].position.z;


	//	}
	//}

	//------------------ インデックスリソースにデータを書き込む ------------------//
	//
	//uint32_t * indexDataSphere = nullptr;
	//indexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSphere));
	//uint32_t index = 0;
	//for (latindex = 0; latindex < kSubdivision; ++latindex) {
	//	for (lonindex = 0; lonindex < kSubdivision; ++lonindex) {
	//		uint32_t start = (latindex * kSubdivision + lonindex) * 6;
	//		uint32_t a = start;
	//		uint32_t b = start + 1;
	//		uint32_t c = start + 2;
	//		uint32_t d = start + 3;
	//		// インデックスの設定
	//		indexDataSphere[index++] = a;
	//		indexDataSphere[index++] = b;
	//		indexDataSphere[index++] = c;

	//		indexDataSphere[index++] = b;
	//		indexDataSphere[index++] = d;
	//		indexDataSphere[index++] = c;
	//	}
	//}
	//indexResourceSphere->Unmap(0, nullptr);

	//------------------ Sprite用の頂点リソースにデータを書き込む ------------------//
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	// 1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f }; //右上
	vertexDataSprite[3].texcoord = { 1.0f,0.0f }; 
	vertexDataSprite[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	vertexDataSprite[4].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[5].texcoord = { 1.0f,1.0f };
	// 書き込むためのアドレスを取得
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	//------------------ インデックスリソースにデータを書き込む ------------------//
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;  // 最初の三角形
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;  // 2つ目の三角形
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;


	////------------------ ビューポート ------------------//
	//D3D12_VIEWPORT viewport{};
	//// クライアント領域のサイズと一緒にして画面全体に表示
	//viewport.Width =  WinApp::kClientWidth;
	//viewport.Height = WinApp::kClientHeight;
	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;
	//// シザー矩形
	//D3D12_RECT scissorRect{};
	//// 基本的にビューポートと同じ矩形が構成されるようにする
	//scissorRect.left = 0;
	//scissorRect.right = WinApp::kClientWidth;
	//scissorRect.top = 0;
	//scissorRect.bottom = WinApp::kClientHeight;

	// CPUで動かすSphere用のTransform
	Transform_h transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	// CPUで動かすSprite用のTransform
	Transform_h transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform_h cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);

	//// ImGuiの初期化。こういうもん
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//ImGui_ImplWin32_Init(winApp_->Gethwnd());
	//ImGui_ImplDX12_Init(device.Get(),
	//	swapChainDesc.BufferCount,
	//	rtvDesc.Format,
	//	srvDescriptorHeap.Get(),
	//	GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptotSizeSRV, 0),
	//	GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptotSizeSRV, 0));

	//　Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon->LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureResource.Get(), mipImages);

	// 2枚目のTexture
	DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(metadata2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = dxCommon->UploadTextureData(textureResource2.Get(), mipImages2);

	//---------------- metaDataを基にSRVの設定 ----------------//
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	// 2枚目のmetaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dtexture
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// SRVを作成DescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxCommon->GetsrvDescriptorHeap().Get(), dxCommon->GetdescriptotSizeSRV(), 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(dxCommon->GetsrvDescriptorHeap().Get(), dxCommon->GetdescriptotSizeSRV(), 1);
	// 2枚目のSRVを作成DescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(dxCommon->GetsrvDescriptorHeap().Get(), dxCommon->GetdescriptotSizeSRV(), 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(dxCommon->GetsrvDescriptorHeap().Get(), dxCommon->GetdescriptotSizeSRV(), 2);
	// 先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの生成
	dxCommon->Getdevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	// 2枚目のSRVの生成
	dxCommon->Getdevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
	// SRV切り替え
	bool useMonsterBall = true;
	// uvTransform行列を単位行列で初期化
	materialData->uvTransform = MakeIdentity4x4();
	materialDataSprite->uvTransform = MakeIdentity4x4();
	// uvTransform用の変数
	UVTransform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	
	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		// windowにメッセージが来てたら最優先で処理させる
		if (winApp_->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		
			input_->Update(winApp_);
			if (input_->TriggerKey(DIK_0)) {
				transform.rotate.y += 0.1f;
			}
			// ここからフレームが始まる
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			//================================ ゲームの処理 ================================//
		//	transform.rotate.y += 0.01f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(winApp_->kClientWidth) / float(winApp_->kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldViewProjectionMatrix;
			//------------------ Sprite用のWorldViewProjectionMatrixを作る ------------------//
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(winApp_->kClientWidth), float(winApp_->kClientHeight), 0.0f, 100.0f);
			Matrix4x4 worldProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			wvpDataSprite->WVP = worldProjectionMatrixSprite;
			wvpDataSprite->World = worldProjectionMatrixSprite;
			//------------------ UVTransform用の行列 ------------------//
			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateMatrixZ(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			// 開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
			ImGui::Begin("Setting");
			ImGui::ColorEdit4("material : Color", &materialData->color.x);
			ImGui::DragFloat3("translateSprite", &transformSprite.translate.x, 1.0f);
			ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			ImGui::ColorEdit4("directonalLight : Color", &directionalLight->color.x);
			//ImGui::ColorEdit4("material : Color", &vertexData->position.x, 0.01f);
			ImGui::DragFloat3("directonalLight : direction", &directionalLight->direction.x, 0.01f);
			directionalLight->direction = Normalize(directionalLight->direction);
			ImGui::DragFloat("directonalLight : intensity", &directionalLight->intensity, 0.01f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("Model RotateX", &transform.rotate.x);
			ImGui::SliderAngle("Model RotateY", &transform.rotate.y);
			ImGui::SliderAngle("Model RotateZ", &transform.rotate.z);
			ImGui::DragFloat("translateX", &transform.translate.x, 0.01f);
			ImGui::DragFloat("translateY", &transform.translate.y, 0.01f);
			ImGui::DragFloat("translateZ", &transform.translate.z, 0.01f);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::End();

			// DirectXの描画準備。全ての描画にグラフィックスコマンドを積む
			dxCommon->PreDraw();

			// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
			spriteCommon->DrawPreference();


			
			// 描画処理の前・ImGuiの内部コマンドを生成する
			ImGui::Render();
			





			// RootSignatureを設定。PSOに設定しているけど別途設定が必要
			dxCommon->GetcommandList()->SetPipelineState(graphicsPipelineState.Get());    	  // PSOを設定
			dxCommon->GetcommandList()->SetGraphicsRootSignature(rootSignature.Get());
			dxCommon->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);	  // VBVを設定
			// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
			dxCommon->GetcommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// IndexBufferView
			//commandList->IASetIndexBuffer(&indexBufferViewSphere);//IBV
			// マテリアルCBufferの場所を設定
			dxCommon->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			// WVP用のCBufferの場所を指定
			dxCommon->GetcommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			// DescriptorTableを設定する  SRVのDescriptorTableの先頭を設定。2はrootParameter[2]
			dxCommon->GetcommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			// DirectionalLight用のCBufferの場所を指定
			dxCommon->GetcommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			
			// Triangleの描画！DrawCall/ドローコール）。3頂点1つのインスタンス。
			dxCommon->GetcommandList()->DrawInstanced(UINT(modelData.verteces.size()), 1, 0, 0);
			dxCommon->GetcommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


			// マテリアルCBufferの場所を指定
			dxCommon->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			// TransformatonMatrixCBuffferの場所を設定
			dxCommon->GetcommandList()->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
			// Spriteの描画！　変更が必要なものだけ変更する
			dxCommon->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBVを設定
			// IndexBufferView
			dxCommon->GetcommandList()->IASetIndexBuffer(&indexBufferViewSprite);//IBV
			// 描画！！！DrawCall/ドローコール）
			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0 );

			// ImGuiの描画コマンド
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetcommandList().Get());
			//------------------ 画面に描く処理は全て終わり、画面に映すので状態を遷移 ------------------//
			

			dxCommon->PostDraw();


	}// ゲームループの終了
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	// Release
	CloseHandle(dxCommon->GetfenceEvent());
	delete input_;
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;
	delete dxCommon;
	delete spriteCommon;
	delete sprite;

	
	return 0;// main関数のリターン
}

