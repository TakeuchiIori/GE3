#include "Sprite.h"
#include "SpriteCommon.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon_ = spriteCommon;

	VertexResource();

	IndexResource();

	MaterialResource();
}

void Sprite::Update()
{


}

void Sprite::VertexResource()
{
	// リソース
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);
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
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f }; //右上
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };//左上
	vertexData[4].texcoord = { 0.0f, 0.0f };
	vertexData[5].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[5].texcoord = { 1.0f,1.0f };
	// 書き込むためのアドレスを取得
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
}

void Sprite::IndexResource()
{
	// リソース
	indexResource_ = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);
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
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 0.0f,0.0f, 0.0f, 0.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();

	TransformResource();
}

void Sprite::TransformResource()
{
	// リソース作成
	transformationMatrixResource_ = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込むためのアドレスを取得して割り当て
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 単位行列を書き込む
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();
}
