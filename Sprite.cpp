#include "Sprite.h"
#include "SpriteCommon.h"
#include "Transform.h"

Sprite::Sprite()
{
}

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon_ = spriteCommon;

	VertexResource();

	IndexResource();

	MaterialResource();

	TransferTexture();

	SetSRV();
}

void Sprite::Update()
{
	Transform_h transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData_->WVP = worldProjectionMatrix;
	transformationMatrixData_->World =worldMatrix;
}

void Sprite::Draw()
{
	// VertexBufferView
	spriteCommon_->GetDxCommon()->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// IndexBufferView
	spriteCommon_->GetDxCommon()->GetcommandList()->IASetIndexBuffer(&indexBufferView_);//IBV

	// マテリアルCBufferの場所を指定
	spriteCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformatonMatrixCBuffferの場所を設定
	spriteCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	// SRVの設定
	spriteCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU); // SRVのパラメータインデックスを変更

	// 描画！！！DrawCall/ドローコール）
	spriteCommon_->GetDxCommon()->GetcommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}


void Sprite::VertexResource()
{
	// リソース
	vertexResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);
	// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	CreateVertex();
}

void Sprite::CreateVertex()
{
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 1枚目の三角形
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };   //左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };     //左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f }; //右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };   //右上
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };  //左上
	vertexData[4].texcoord = { 0.0f, 0.0f };
	vertexData[5].position = { 640.0f,360.0f,0.0f,1.0f }; //右下
	vertexData[5].texcoord = { 1.0f,1.0f };
	// 書き込むためのアドレスを取得
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
}

void Sprite::IndexResource()
{
	// リソース
	indexResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
	// リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースサイズはもとの頂点のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	CreateIndex();
}

void Sprite::CreateIndex()
{
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;  // 最初の三角形
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;  // 2つ目の三角形
	indexData[4] = 3;
	indexData[5] = 2;
}

void Sprite::MaterialResource()
{
	// リソース作成
	materialResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();


	TransformResource();
}

void Sprite::TransformResource()
{
	// リソース作成
	transformationMatrixResource_ = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込むためのアドレスを取得して割り当て
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 単位行列を書き込む
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();
}

void Sprite::TransferTexture()
{
	mipImages[0] = spriteCommon_->GetDxCommon()->LoadTexture("Resources/uvChecker.png");
	textureResource[0] = spriteCommon_->GetDxCommon()->CreateTextureResource(metadata);
	intermediateResource[0] = spriteCommon_->GetDxCommon()->UploadTextureData(textureResource[0].Get(), mipImages[0]);

	// 2枚目のTexture
	mipImages[1] = spriteCommon_->GetDxCommon()->LoadTexture("Resources/monsterball.png");
	textureResource[1] = spriteCommon_->GetDxCommon()->CreateTextureResource(metadata2);
	intermediateResource[1] = spriteCommon_->GetDxCommon()->UploadTextureData(textureResource[1].Get(), mipImages[1]);
}

void Sprite::SetSRV()
{
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
	textureSrvHandleCPU = spriteCommon_->GetDxCommon()->GetCPUDescriptorHandle(spriteCommon_->GetDxCommon()->GetsrvDescriptorHeap().Get(), spriteCommon_->GetDxCommon()->GetdescriptotSizeSRV(), 1);
	textureSrvHandleGPU = spriteCommon_->GetDxCommon()->GetGPUDescriptorHandle(spriteCommon_->GetDxCommon()->GetsrvDescriptorHeap().Get(), spriteCommon_->GetDxCommon()->GetdescriptotSizeSRV(), 1);

	// 先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの生成
	spriteCommon_->GetDxCommon()->Getdevice()->CreateShaderResourceView(textureResource[0].Get(), &srvDesc, textureSrvHandleCPU);
	// 2枚目のSRVの生成

	// SRV切り替え　　使っていないためコメントアウト
	
	//bool useMonsterBall = true;

}
