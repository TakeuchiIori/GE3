#include "SceneManager.h"
#include <assert.h>
SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new SceneManager;
	}
	return instance;
}

void SceneManager::Finalize()
{
	// 最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
	delete instance;
	instance = nullptr;
}

void SceneManager::Initialize()
{
}

void SceneManager::Update()
{
	//========== TODO : シーン切り替え機構 ///==========///
	// 次のシーンがあるなら
	//・旧シーンの終了
	//・シーン切り替え
	if (nextScene_) {
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		scene_ = nextScene_;
		nextScene_ = nullptr;

		// シーンマネージャをセット
		scene_->SetSceneManager(this);

		scene_->Initialize();

	}
	// 実行中のシーンを更新
	scene_->Update();
}

void SceneManager::Draw()
{
	// 描画
	scene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// 次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}
