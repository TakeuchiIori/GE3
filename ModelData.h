#pragma once
#include "MaterialData.h"
#include "Vector4.h"
#include "VertexData.h"
#include <vector>
#include <stdint.h>
struct ModelData {
	std::vector<VertexData> verteces;
	MaterialData material;
};