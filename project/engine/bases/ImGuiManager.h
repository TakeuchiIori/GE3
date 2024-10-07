#pragma once
#include <wrl.h>
#include <d3d12.h>


class WinApp;
class DirectXCommon;
class ImGuiManager
{
public: // メンバ関数

	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp,DirectXCommon* dxCommon);

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 画面への描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デスクリプターヒープ生成
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// DirextX12初期化
	/// </summary>
	void InitialzeDX12();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();


private:
	static ImGuiManager* instance;
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(ImGuiManager&) = delete;
	ImGuiManager& operator = (ImGuiManager&) = delete;

	// ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;
	// SRV用デスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

