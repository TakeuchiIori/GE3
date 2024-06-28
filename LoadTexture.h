#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include "Convertstring.h"
#include <cstdint>
#include <string>
#include <cassert>
DirectX::ScratchImage LoadTexture(const std::string& filePath);