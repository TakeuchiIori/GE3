#include "CreateDepthStencilTextureResource.h"
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										// Textureのは幅
	resourceDesc.Height = height;									// Textureの高さ
	resourceDesc.MipLevels = 1;										// mipmapの数
	resourceDesc.DepthOrArraySize = 1;								//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// Depthstencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;								//サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencilとして使う通知
	//.利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;					// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;						// 1.0f (最大値)　でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// フォーマット。Resourceと合わせる

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc,// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
		&depthClearValue,// clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
		assert(SUCCEEDED(hr));
	
		return resource;
};