#include "MyGame.h"
#include "ParticleManager.h"

void MyGame::Initialize()
{
	// 基盤の初期化
	Framework::Initialize();

	// シーンファクトリを生成し、 シーンマネージャに最初のシーンをセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->ChangeScene("GAME");
	// パーティクルマネージャ生成
	ParticleManager::GetInstance()->Initialize(srvManager_);
	
}

void MyGame::Finalize()
{
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
	SceneManager::GetInstance()->Draw();
#ifdef _DEBUG
	collisionManager_->Draw();
#endif

	// ImGui描画
	imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
