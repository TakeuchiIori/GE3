#pragma once

class WinApp;
class DirectXCommon;
class ImGuiManager
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp,DirectXCommon* dxCommon);
};

