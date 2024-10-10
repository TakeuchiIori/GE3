#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include <memory>
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
	/// 初期化
	/// </summary>
	void Initialize();

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
	SceneManager(SceneManager&) = delete;
	SceneManager& operator = (SceneManager&) = delete;
	// 今のシーン（実行中シーン）
	BaseScene* scene_;
	// 次のシーン
	BaseScene* nextScene_;
	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

};

