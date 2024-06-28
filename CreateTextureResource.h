#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <d3d12.h>
#include <vector>
#include "wrl.h"
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

