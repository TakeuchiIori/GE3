#include "Framework.h"

void Framework::Initialize()
{
	// ウィンドウ生成
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	// 入力生成
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	// DirectX生成
	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(winApp_);

	// サウンド生成
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	// ImGui生成
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize(winApp_, dxCommon_);

	// SRVマネージャの生成
	srvManager_ = SrvManager::GetInstance();
	srvManager_->Initialize(dxCommon_);


	// テクスチャマネージャの生成
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(dxCommon_, srvManager_);

	// スプライト共通部の生成
	spriteCommon_ = SpriteCommon::GetInstance();
	spriteCommon_->Initialize(dxCommon_);

	// 3Dオブジェクト共通部の生成
	object3dCommon_ = Object3dCommon::GetInstance();
	object3dCommon_->Initialize(dxCommon_);

	// 3Dモデルマネージャの生成
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialze(dxCommon_);


}

void Framework::Finalize()
{
	// 各解放処理
	imguiManager_->Finalize();
	SceneManager::GetInstance()->Finalize();
	textureManager_->Finalize();
	srvManager_->Finalize();
	audio_->Finalize();
	input_->Finalize();
	winApp_->Finalize();
	winApp_ = nullptr;	
}

void Framework::Update()
{
	// ImGui受付開始
	imguiManager_->Begin();
	// 入力は初めに更新
	input_->Update(winApp_);
	// シーン全体の更新
	SceneManager::GetInstance()->Update();
	// ImGui受付終了
	imguiManager_->End();
}

void Framework::Run()
{
	// ゲームの生成
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
