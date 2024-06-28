#pragma once
#include <Windows.h>
#include <cassert>
#include <string>
#include <format>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "math/Vector4.h"
#include "CompileShader.h"
#include "Convertstring.h"
#include "wrl.h"
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t  sizeInBytes);
