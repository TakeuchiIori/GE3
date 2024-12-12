#include "Line.h"
#include "DirectXCommon.h"
#include "LineManager.h"
#include "camera/Camera.h"
void Line::Initialize()
{
	lineManager_ = LineManager::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();

	CrateMaterialResource();

	CrateVetexResource();

	CreateTransformResource();

}

void Line::DrawLine(const Vector3& start, const Vector3& end)
{
	// 前回のデータと異なる場合のみ頂点を更新
	if (start != lastStart_ || end != lastEnd_) {
		UpdateVertices(start, end);
		lastStart_ = start;
		lastEnd_ = end;
	}


	// WVP行列を更新
	if (camera_) {
		transformationMatrix_->WVP = camera_->GetViewProjectionMatrix();
	}
	else {
		transformationMatrix_->WVP = MakeIdentity4x4();
	}

	// GPUに頂点バッファを設定して描画
	auto commandList = dxCommon_->GetCommandList();
	commandList->SetGraphicsRootSignature(lineManager_->GetRootSignature_().Get());
	commandList->SetPipelineState(lineManager_->GetGraphicsPiplineState().Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->DrawInstanced(2, 1, 0, 0); // ラインは2つの頂点で構成
}

void Line::UpdateVertices(const Vector3& start, const Vector3& end)
{
	// 頂点データの設定
	VertexData vertices[2];
	vertices[0].position = { start.x, start.y, start.z, 1.0f }; // 始点
	vertices[1].position = { end.x, end.y, end.z, 1.0f };       // 終点

	// 頂点データをGPUリソースにコピー
	void* mappedData = nullptr;
	vertexResource_->Map(0, nullptr, &mappedData);
	memcpy(mappedData, vertices, sizeof(vertices));
	vertexResource_->Unmap(0, nullptr);
}

void Line::CrateVetexResource()
{
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * 2);
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * 2);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Line::CrateMaterialResource()
{
	// 必要なサイズを256バイト単位にアラインメント
	const UINT materialSize = (sizeof(MaterialData) + 255) & ~255;

	materialResource_ = dxCommon_->CreateBufferResource(materialSize);
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void Line::CreateTransformResource()
{
	// WVP行列リソースを作成
	const UINT transformationSize = (sizeof(Matrix4x4) + 255) & ~255;
	transformationResource_ = dxCommon_->CreateBufferResource(transformationSize);
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix_));
	transformationMatrix_->WVP = MakeIdentity4x4();
}
