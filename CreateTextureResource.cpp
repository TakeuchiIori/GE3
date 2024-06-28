#include "CreateTextureResource.h"
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
	// 1. medataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									 // Textureの幅
	resourceDesc.Height = UINT(metadata.height);								 // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						 // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					 // 奥行き　or　配列Textureの配列数
	resourceDesc.Format = metadata.format;										 // TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											 // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		 // textureの次元数。普段使っているのは２次元
	// 2. 利用するHeapの設定。非常に特殊な運用。 02_04で一般ていなケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;								 // 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;		 // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;					 // プロセッサの近くに配置
	// Resourceの生成　( VRAM )
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,						// heapの設定
		D3D12_HEAP_FLAG_NONE,					// heaoの特殊な設定。特になし。
		&resourceDesc,							// Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,			// データ移送される設定
		nullptr,								// Clear最低値。使わないのでnullptr
		IID_PPV_ARGS(&resource));				// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;

} 