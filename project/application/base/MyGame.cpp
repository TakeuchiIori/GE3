#include "MyGame.h"

void MyGame::Initialize()
{
	// 基盤の初期化
	Framework::Initialize();

	// ゲームの初期化
	scene_ = new GameScene();
	scene_ ->Initialize();
	
}

void MyGame::Finalize()
{
	// 各解放処理
	CloseHandle(dxCommon_->GetfenceEvent());
	scene_ ->Finalize();
	delete scene_ ;
	Framework::Finalize();
}

void MyGame::Update()
{
	// 基盤の更新
	Framework::Update();
	
	// ゲームの更新
	scene_ ->Update();

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
	scene_ ->Draw();

	// ImGui描画
	imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
