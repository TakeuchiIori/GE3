#include "DescriptorHeap.h"
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;							 //レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors;			 //ダブルバッファ用に2つ。多くても別に構わない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : 
	D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc,
	IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}