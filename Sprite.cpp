#include "Sprite.h"
#include "SpriteCommon.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon_ = spriteCommon;

	VertexResource();

	IndexResource();
}

void Sprite::VertexResource()
{
	// リソース
	vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);
	// リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	CreateVertex();
}

void Sprite::CreateVertex()
{
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
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
	indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);
	// リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズはもとの頂点のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	CreateIndex();
}

void Sprite::CreateIndex()
{
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;  // 最初の三角形
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;  // 2つ目の三角形
	indexData[4] = 3;
	indexData[5] = 2;
}
