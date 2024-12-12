#include "SceneFactory.h"
// Engine
#include "MainScenes/Game/GameScene.h"
#include "MainScenes/Title/TitleScene.h"
//#include "MainScenes/Result/ResultScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーンを生成
    BaseScene* newScene = nullptr;

    if (sceneName == "TITLE") {
        newScene = new TitleScene();
    }
    else if (sceneName == "GAME") {
        newScene = new GameScene();
    }

    return newScene;
}
