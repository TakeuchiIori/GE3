#pragma once
#include "Convertstring.h"
// 文字コードユーティリティ
namespace StringUtility
{
	// stringをwstirngに変換
	std::wstring ConvertString(const std::string& str);

	// wstringをstirngに変換
	std::string ConvertString(const std::wstring& str);
}

