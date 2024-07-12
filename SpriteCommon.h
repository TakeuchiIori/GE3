#pragma once
#include "DirectXCommon.h"
class SpriteCommon
{
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 初期化
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 初期化
	/// </summary>
	void CreateGraphicsPipeline();

private: // メンバ変数
	DirectXCommon* dxCommon_;

};

