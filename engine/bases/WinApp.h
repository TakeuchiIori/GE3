#pragma once
#include "Windows.h"
#include <cstdint>
#include "imgui.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
class WinApp
{
public: // 静的メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
		WPARAM wparam, LPARAM lparam);
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	bool ProcessMessage();

public: // アクセッサ
	HINSTANCE Gethinstance() { return wc.hInstance; }
	HWND Gethwnd() { return hwnd; }

public: // 定数
	// クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private: // メンバ変数
	// ウィンドウクラスの設定
	WNDCLASS wc{};
	// ウィンドウハンドル
	HWND hwnd;

};

