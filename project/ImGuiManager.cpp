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

	// デスクリプタヒープ生成
	CreateDescriptorHeap();



}

void ImGuiManager::CreateDescriptorHeap()
{
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;							 
	desc.NumDescriptors =1;			 
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// デスクリプタヒープ生成
	HRESULT hr = dxCommon_->Getdevice()->CreateDescriptorHeap(&desc,IID_PPV_ARGS(&srvHeap_));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
}
