#pragma once

// C++
#include <optional>
#include <map>
#include <vector>
#include <d3d12.h>
#include <string>
#include <wrl.h>

// Engine
#include "Joint.h"
#include "Node.h"
#include "Skeleton.h"

// Math
#include "Quaternion.h"
#include "Vector3.h"


class SkinCluster
{

public:



private:


	std::vector<Matrix4x4> inverseBindposeMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	uint32_t influSrvIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	uint32_t srvIndex;

};

