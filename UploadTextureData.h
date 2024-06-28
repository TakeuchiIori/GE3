#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include "wrl.h"
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// 読み込んだデータからDirectX12用のSubresourceの配列を作成
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// Subresourceの数を基に、コピー元となるIntermediateResourceに必要なサイズを計算する。
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// 計算したサイズでIntermediateResourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	// IntermediateResourceにSubresourceのデータ書き込み、tectureに転送するコマンドを積む
	UpdateSubresources(commandList, texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	//Tetureへの転送後は利用できるよう、D3D12_RESOURCE_ STATE_ COPY_DESTからD3012_RESOURCE_STATE GENERIC_READへResourcestateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}
//void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
//{
//	// Meta情報を取得
//	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//	// 全MipMapについて
//	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
//		// MipMapLevelを指定して各Imageを取得
//		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
//		// Textureに転送
//		HRESULT hr = texture->WriteToSubresource(
//			UINT(mipLevel),				
//			nullptr,					// 全領域へコピー
//			img->pixels,				// 元データアドレス
//			UINT(img->rowPitch),		// １ラインサイズ
//			UINT(img->slicePitch)		// 1枚サイズ
//		);
//		assert(SUCCEEDED(hr));
//	}
//}