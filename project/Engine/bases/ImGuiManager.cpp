#include "ImGuiManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#ifdef _DEBUG
#include "imgui.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#endif



ImGuiManager* ImGuiManager::instance = nullptr;
ImGuiManager* ImGuiManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ImGuiManager;
	}
	return instance;
}

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
#ifdef _DEBUG
	// メンバ変数に引数を渡す
	dxCommon_ = dxCommon;

	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// Win32用初期化
	ImGui_ImplWin32_Init(winApp->GetHwnd());

	// デスクリプタヒープ生成
	CreateDescriptorHeap();

	// DirectX12の初期化
	InitialzeDX12();

	// 変更したエディター呼び出し
	CustomizeEditor();
	

#endif
}

void ImGuiManager::Begin()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// DockSpaceの設定

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f)); // ウィンドウ全体をカバーする

	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::SetNextWindowBgAlpha(0.0f); // 背景を透明にする
	ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGui::Begin("MainDockSpace", nullptr, dockspace_flags);

	// DockSpaceの作成
	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();
#endif

}

void ImGuiManager::End()
{
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render();
#endif
}

void ImGuiManager::Draw()
{
#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList().Get();

	// デスクリプターヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
	
#endif
}

void ImGuiManager::CreateDescriptorHeap()
{
#ifdef _DEBUG
	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// デスクリプタヒープ生成
	HRESULT hr = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
#endif // DEBUG
}

void ImGuiManager::InitialzeDX12()
{
#ifdef _DEBUG
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice().Get(),
		static_cast<int> (dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_.Get(),
		srvHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvHeap_->GetGPUDescriptorHandleForHeapStart()
	); 
#endif // DEBUG
}

void ImGuiManager::CustomizeEditor()
{
#ifdef _DEBUG
	// エディター同士をドッキング
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	// メインDockSpaceの設定
	//ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	//	ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	//	ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// ウィンドウ全体をカバーするDockSpaceの作成
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f)); // ウィンドウ位置を(0,0)に設定
	ImGui::SetNextWindowSize(io.DisplaySize);    // ウィンドウサイズを全画面に設定


	// フォントファイルのパスとサイズを指定してフォントをロードする
	io.Fonts->AddFontFromFileTTF(
		"Resources/Fonts/FiraMono-Regular.ttf", 14.0f // フォントファイルのパスとフォントサイズ
	);
	// 標準フォントを追加する
	io.Fonts->AddFontDefault();


	ImGuiStyle& style = ImGui::GetStyle();

	// カラースキームのカスタマイズ
	ImVec4* colors = style.Colors;
	/*========================================================
							ドックスペース
	========================================================*/

	// DockSpace背景色を透明に設定
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 完全に透明
	// パディングとスペーシングの調整
	style.WindowPadding = ImVec2(0.0f, 0.0f); // ウィンドウ内の余白をゼロに

	//style.FramePadding = ImVec2(0.0f, 0.0f);  // フレーム内の余白をゼロに
	//style.ItemSpacing = ImVec2(0.0f, 0.0f);   // アイテム間のスペースをゼロに
	//style.CellPadding = ImVec2(0.0f, 0.0f);   // テーブルセル内の余白をゼロに
	//style.WindowBorderSize = 0.0f;            // ウィンドウの境界線幅をゼロに
	//style.ChildBorderSize = 0.0f;             // 子ウィンドウの境界線幅をゼロに
	//style.PopupBorderSize = 0.0f;             // ポップアップの境界線幅をゼロに
	//style.FrameBorderSize = 0.0f;             // フレームの境界線幅をゼロに
	//// DockNodeのスペーシングを調整
	//style.TabBorderSize = 0.0f;               // タブの境界線幅をゼロに
	////style.TabRounding = 0.0f;                 // タブの角丸をゼロに


	/*========================================================
							ImGui
	========================================================*/

	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);    // 背景色 (ダークグレー)
	colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);  // タイトルバー (暗い灰色)
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);    // アクティブなタイトルバー (少し明るい灰色)
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);    // 折りたたまれたタイトルバー
	colors[ImGuiCol_Button] = ImVec4(0.2f, 0.22f, 0.3f, 1.0f);    // ボタン (青灰色)
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.35f, 0.45f, 1.0f);   // ホバーしたボタン (明るい青灰色)
	colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.28f, 0.35f, 1.0f);  // 押されたボタン (少し暗めの青灰色)
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);  // 入力欄やフレーム背景
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);    // ホバーしたフレーム背景
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.3f, 1.0f);   // アクティブなフレーム背景
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.6f, 0.85f, 1.0f);   // チェックマーク (青アクセント)
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.6f, 0.85f, 1.0f);   // スライダーつまみ (青アクセント)
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.7f, 0.95f, 1.0f);   // アクティブなスライダーつまみ
	colors[ImGuiCol_Header] = ImVec4(0.2f, 0.25f, 0.3f, 1.0f);    // ヘッダー背景
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.35f, 0.4f, 1.0f);    // ホバーしたヘッダー
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.3f, 0.35f, 1.0f);   // アクティブなヘッダー
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);  // タブ背景
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.3f, 0.4f, 1.0f);    // ホバーしたタブ
	colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.25f, 0.35f, 1.0f);   // アクティブなタブ
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.3f, 1.0f);   // セパレーター
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.4f, 0.45f, 1.0f);   // ホバーしたセパレーター
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.5f, 0.55f, 1.0f);   // アクティブなセパレーター
	colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);     // テキスト (白)
	colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);     // 無効化されたテキスト (灰色)
	colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);    // 境界線
	colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);     // 境界線の影



	// スタイルの変更
	style.WindowRounding = 15.0f;      // ウィンドウの角丸
	style.FrameRounding = 4.0f;        // フレームの角丸
	style.ScrollbarSize = 15.0f;       // スクロールバーのサイズ
	//style.FramePadding = ImVec2(10, 10); // フレーム内のパディング

	// ImGuiのスタイルを設定
	//ImGui::StyleColorsClassic();

#endif;
}


void ImGuiManager::Finalize()
{
#ifdef _DEBUG
	// 後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// デスクリプターヒープを解放
	srvHeap_.Reset();

	delete instance;
	instance = nullptr;
#endif // DEBUG
}
