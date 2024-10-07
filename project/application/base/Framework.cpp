#include "Framework.h"

void Framework::Initialize()
{
	winApp_ = new WinApp();
	winApp_->Initialize();

	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_);

	audio_ = Audio::GetInstance();
	audio_->Initialize();

	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize(winApp_, dxCommon_);
}

void Framework::Finalize()
{
	audio_->Finalize();
	imguiManager_->Finalize();
	input_->Finalize();
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;
	dxCommon_->Finalize();
	
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
