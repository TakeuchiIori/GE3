#include "GameScene.h"
#include "SceneManager.h"
#include "Input.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG

void GameScene::Initialize()
{
	///============ モデル読み込み ============///
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	///============ カメラ初期化 ============///
	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.0f,0.0f,0.0f });
	camera_->SetTranslate({ 0.0f,4.0f,-10.0f });
	Object3dCommon::Getinstance()->SetDefaultCamera(camera_.get());

	///============ スプライト初期化 ============///
	std::string textureFilePath[2] = { "Resources./monsterBall.png" ,"Resources./uvChecker.png" };
	for (uint32_t i = 0; i < 1; ++i) {
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(textureFilePath[0]);
		// 移動テスト
		Vector2 position;
		position.x = i * 200.0f;
		position.y = 0.0f;
		sprite->SetPosition(position);
		sprite->SetSrvManager(SrvManager::GetInstance());

		// 初期色の設定
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		sprite->SetColor(color);
		if (i % 2 != 0) {
			sprite->ChangeTexture(textureFilePath[0]);
		}
		else {
			sprite->ChangeTexture(textureFilePath[0]);
		}
		sprites.push_back(std::move(sprite));
	}

	///============ オブジェクト初期化 ============///
	uint32_t numObjects = 2;
	for (uint32_t i = 0; i < numObjects; ++i) {
		auto object = std::make_unique<Object3d>();
		object->Initialize(Object3dCommon::Getinstance());
		Vector3 position;
		if (i == 0) {
			position.x = -2;
			object->SetModel("plane.obj");
		}
		else if (i == 1) {
			position.x = 3;
			object->SetModel("axis.obj");
		}
		position.y = 3;
		position.z = 0.0f;
		object->SetPosition(position);
		object3ds.push_back(std::move(object));
	}

	soundData = Audio::GetInstance()->LoadWave("Resources./fanfare.wav");
	
}

void GameScene::Finalize()
{
	/// 解放処理
	Audio::GetInstance()->SoundUnload(Audio::GetInstance()->GetXAudio2(), &soundData);
}

void GameScene::Update()
{
	// ENTERキーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え依頼
		sceneManager_->ChangeScene("TITLE");
	}

	// 2Dスプライトの更新
	for (size_t i = 0; i < sprites.size(); ++i) {
		auto& sprite = sprites[i];
		sprite->Update();
		float rotation = sprite->GetRotation();
		rotation += 0.01f;
		//sprite->SetRotation(rotation);
		Vector2 size = sprite->GetSize();
		size.x += 0.01f;
		size.y += 0.01f;
		//sprite->SetSize(size);
		Vector4 color = sprite->GetColor();
		color.x += 0.01f;
		if (color.x > 1.0f) {
			color.x -= 1.0f;
		}
		//sprite->SetColor(color);
		Vector2 position = sprite->GetPosition();
		sprite->SetPosition(position);
	}

	// 3Dオブジェクトの更新
	for (int i = 0; i < object3ds.size(); ++i) {
		auto& obj = object3ds[i];
		
		Vector3 rotate = obj->GetRotation();
		if (i == 0) {
			
#ifdef _DEBUG
			ImGui::Begin("OBJ");
			ImGui::ColorEdit4("material : Color", &color.x);
			ImGui::End();
#endif // DEBUG
			obj->SetColor(color);
			rotate.y += 0.01f;
		}
		else if (i == 1) {
			rotate.z += 0.01f;
		}
		obj->SetRotation(rotate);
		obj->Update();
	}


	// デフォルトカメラの更新
	camera_->Update();
}

void GameScene::Draw()
{
	///======================  描画準備  ========================///
	
	/// 3Dオブジェクトの描画準備
	Object3dCommon::Getinstance()->DrawPreference();
	/// 2DSpriteの描画準備
	SpriteCommon::Getinstance()->DrawPreference();

	///======================  実際に描画  ========================///

	// 2Dスプライト
	for (auto& sprite : sprites) {
		sprite->Draw();
	}
	// 3Dオブジェクト
	for (auto& obj : object3ds) {
		obj->Draw();
	}

}
