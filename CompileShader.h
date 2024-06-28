#pragma once
#include <Windows.h>
#include <cassert>
#include <format>
#include <cstdint>
#include <dxcapi.h>

//#pragma comment(lib,"dxguid.lib")
//#pragma comment(lib,"d3d12.lib")
//#pragma comment(lib,"dxgi.lib")
//#pragma comment(lib,"dxcompiler.lib")
IDxcBlob* CompilerShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler);

