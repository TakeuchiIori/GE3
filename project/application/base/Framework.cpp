#include "Framework.h"

void Framework::Initialize()
{
	// ウィンドウ初期化
	winApp_ = new WinApp();
	winApp_->Initialize();

	// 入力初期化
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	// DirectX初期化
	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_);

	// サウンド初期化
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	// ImGui初期化
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize(winApp_, dxCommon_);

	// SRVマネージャの初期化
	srvManager_ = SrvManager::GetInstance();
	srvManager_->Initialize(dxCommon_);

	// スプライト共通部の初期化
	spriteCommon_ = SpriteCommon::Getinstance();
	spriteCommon_->Initialize(dxCommon_);

	// テクスチャマネージャの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(dxCommon_, srvManager_);

	// 3Dオブジェクト共通部の初期化
	object3dCommon_ = Object3dCommon::Getinstance();
	object3dCommon_->Initialize(dxCommon_);

	// 3Dモデルマネージャの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialze(dxCommon_);
}

void Framework::Finalize()
{
	// 各解放処理
	modelManager_->Finalize();
	object3dCommon_->Finalize();
	textureManager_->Finalize();
	spriteCommon_->Finalize();
	srvManager_->Finalize();
	imguiManager_->Finalize();
	audio_->Finalize();
	dxCommon_->Finalize();
	input_->Finalize();
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;	
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
