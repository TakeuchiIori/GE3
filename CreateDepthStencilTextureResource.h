#pragma once
#include <d3d12.h>
#include <stdint.h>
#include <cassert>
#include "wrl.h"
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);