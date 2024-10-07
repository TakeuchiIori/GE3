#include "Framework.h"

void Framework::Initialize()
{
	winApp_ = new WinApp();
	winApp_->Initialize();

	input_ = new Input();
	input_->Initialize(winApp_);

	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);

	audio_ = new Audio();
	audio_->Initialize();

	imguiManager_ = new ImGuiManager();
	imguiManager_->Initialize(winApp_, dxCommon_);
}

void Framework::Finalize()
{
	imguiManager_->Finalize();
	delete input_;
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;
	delete dxCommon_;
	delete audio_;
}

void Framework::Update()
{
	if (winApp_->ProcessMessage()) {
		endRequst_ = true;
	}
	imguiManager_->Begin();
	input_->Update(winApp_);
}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

	while (true) // ゲームループ
	{
		// 毎フレーム更新
		Update();
		// 終了リクエストが着たら抜ける
		if (IsEndRequst()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}
