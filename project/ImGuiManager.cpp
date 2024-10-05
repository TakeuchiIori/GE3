#include "ImGuiManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "WinApp.h"
#include "DirectXCommon.h"
void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// Win32用初期化
	ImGui_ImplWin32_Init(winApp->Gethwnd());
}
