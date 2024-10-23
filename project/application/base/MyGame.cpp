#include "MyGame.h"

void MyGame::Initialize()
{
	// 基盤の初期化
	Framework::Initialize();

	// シーンファクトリを生成し、 シーンマネージャに最初のシーンをセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("GAME");
	
}

void MyGame::Finalize()
{
	// 各解放処理
	//CloseHandle(dxCommon_->GetFenceEvent());
	Framework::Finalize();
}

void MyGame::Update()
{
	// 基盤の更新
	Framework::Update();
}

void MyGame::Draw()
{
	// DirectXの描画準備
	dxCommon_->PreDraw();
	// Srvの描画準備
	srvManager_->PreDraw();
	

	// ゲームの描画
	sceneManager_->Draw();

	// ImGui描画
	imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
