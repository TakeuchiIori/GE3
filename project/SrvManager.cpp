#include "SrvManager.h"
#include "DirectXCommon.h"
void SrvManager::Initialize(DirectXCommon* dxCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->dxCommon_ = dxCommon;

	// デスクリプタヒープの生成
	descriptorHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->Getdevice().Get(),D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVcount_,true);
	// デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ = dxCommon_->Getdevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);



}

uint32_t SrvManager::Allocate()
{
	// 上限に達していないかチェックしてassert
	assert(kMaxSRVcount_ > useIndex_);

	// return する番号を一旦記録しておく
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 上で記録して番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUSRVDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE  handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUSRVDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE  handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}
