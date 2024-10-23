#include "GameScene.h"
#include "SceneManager.h"
#include "Input.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include <TextureManager.h>

void GameScene::Initialize()
{
	///============ モデル読み込み ============///
	ModelManager::GetInstance()->LoadModel("float_body.obj");

	///============ カメラ初期化 ============///
	camera_ = std::make_unique<Camera>();

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	///============ オブジェクト初期化 ============///
	player_ = std::make_unique<Player>();
	player_->Initailize();

	test_ = std::make_unique<Object3d>();
	test_->Initialize(Object3dCommon::GetInstance());
	test_->SetModel("float_body.obj");
	testWorldTransform_.Initialize();
}

void GameScene::Finalize()
{ 
	// シーン専用のリソース解放処理

}

void GameScene::Update()
{
	// プレイヤーの更新
	player_->Update();
	
	// プレイヤーの位置を取得
	Vector3 playerPos = player_->GetPosition();
	camera_->FollowCamera(playerPos);

	testWorldTransform_.TransferMatrix();
	// カメラの更新
	camera_->Update();
	
}



void GameScene::Draw()
{
	///======================  描画準備  ========================///
	
	/// 3Dオブジェクトの描画準備
	Object3dCommon::GetInstance()->DrawPreference();
	/// 2DSpriteの描画準備
	SpriteCommon::Getinstance()->DrawPreference();

	///======================  実際に描画  ========================///
	player_->Draw();
	test_->Draw(testWorldTransform_);
}
