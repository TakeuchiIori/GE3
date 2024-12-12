#pragma once
// C++
#include <memory>
#include <mutex>


// Engine
#include "scene/BaseScene.h"
#include "utility/AbstractSceneFactory.h"

class SceneManager
{
public: // メンバ関数
	// シングルトン
	static SceneManager* GetInstance();
	void Finalize();

	// コンストラクタ
	// デストラクタ
	SceneManager() = default;
	~SceneManager() = default;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 次シーン予約
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);

public: // アクセッサ
	// シーンファクトリーのsetter
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	
	
private:
	static std::unique_ptr<SceneManager> instance;
	static std::once_flag initInstanceFlag;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator = (SceneManager&) = delete;
	// 今のシーン（実行中シーン）
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

};

