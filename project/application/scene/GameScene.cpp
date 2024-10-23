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
	/*camera_->SetRotate({ 0.0f,0.0f,0.0f });
	camera_->SetTranslate({ 0.0f,4.0f,-20.0f });*/

	Object3dCommon::Getinstance()->SetDefaultCamera(camera_.get());

	///============ オブジェクト初期化 ============///
	uint32_t numObjects = 2;
	for (uint32_t i = 0; i < numObjects; ++i) {
		auto object = std::make_unique<Object3d>();
		object->Initialize(Object3dCommon::Getinstance());
		Vector3 position;
		if (i == 0) {
			position.x = -2;
			object->SetModel("float_body.obj");
		}
		position.y = 3;
		position.z = 0.0f;
		object->SetPosition(position);
		object3ds.push_back(std::move(object));
	}

	player_ = std::make_unique<Player>();
	player_->Initailize();

}

void GameScene::Finalize()
{ 
	// シーン専用のリソース解放処理
	for (auto& obj : object3ds) {
		obj.reset();
	}
	object3ds.clear();
}

void GameScene::Update()
{
	// プレイヤーの更新
	player_->Update();

	// プレイヤーの位置を取得
	Vector3 playerPos = player_->GetPosition();
	camera_->FollowCamera(playerPos);

	
	// カメラの更新
	camera_->Update();

	// 3Dオブジェクトの更新
	for (int i = 0; i < object3ds.size(); ++i) {
		auto& obj = object3ds[i];

		Vector3 rotate = obj->GetRotation();
		if (i == 0) {
			rotate.y += 0.01f;
		}
		else if (i == 1) {
			rotate.z += 0.01f;
		}
		obj->SetRotation(rotate);
		obj->Update();
	}
	
}



void GameScene::Draw()
{
	///======================  描画準備  ========================///
	
	/// 3Dオブジェクトの描画準備
	Object3dCommon::Getinstance()->DrawPreference();
	/// 2DSpriteの描画準備
	SpriteCommon::Getinstance()->DrawPreference();

	///======================  実際に描画  ========================///

	// 3Dオブジェクト
	for (auto& obj : object3ds) {
		obj->Draw();
	}

	player_->Draw();
}
