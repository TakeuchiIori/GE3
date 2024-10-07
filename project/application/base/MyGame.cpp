#include "MyGame.h"

void MyGame::Initialize()
{
	// 基盤の初期化
	Framework::Initialize();

	// ゲームの初期化
	BaseScene* scene_ = new TitleScene();
	// シーンマネージャに最初のシーンをセット
	sceneManager_->SetNextScene(scene_);
	
}

void MyGame::Finalize()
{
	// 各解放処理
	CloseHandle(dxCommon_->GetfenceEvent());
	Framework::Finalize();
}

void MyGame::Update()
{
	// 基盤の更新
	Framework::Update();

	// ImGui受付終了
	imguiManager_->End();
}

void MyGame::Draw()
{
	// Srvの描画準備
	srvManager_->PreDraw();
	// DirectXの描画準備
	dxCommon_->PreDraw();

	// ゲームの描画
	sceneManager_->Draw();

	// ImGui描画
	imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
