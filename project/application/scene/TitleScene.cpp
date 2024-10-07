#include "TitleScene.h"
void TitleScene::Initialize()
{
	///============ モデル読み込み ============///
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	///============ カメラ初期化 ============///
	camera_ = new Camera();
	camera_->SetRotate({ 0.0f,0.0f,0.0f });
	camera_->SetTranslate({ 0.0f,4.0f,-10.0f });
	Object3dCommon::Getinstance()->SetDefaultCamera(camera_);

	///============ スプライト初期化 ============///
	std::string textureFilePath[2] = { "Resources./monsterBall.png" ,"Resources./uvChecker.png" };
	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(SpriteCommon::Getinstance(), textureFilePath[1]);
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
			sprite->ChangeTexture(textureFilePath[1]);
		}
		sprites.push_back(sprite);
	}



	///============ オブジェクト初期化 ============///
	uint32_t numObjects = 2;
	for (uint32_t i = 0; i < numObjects; ++i) {
		Object3d* object = new Object3d();
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
		object3ds.push_back(object);
	}

	soundData = Audio::GetInstance()->LoadWave("Resources./fanfare.wav");
	
}

void TitleScene::Finalize()
{
	/// 各解放処理
	for (auto& obj : object3ds) {
		delete obj;
	}
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	Object3dCommon::Getinstance()->Finalize();
	Audio::GetInstance()->SoundUnload(Audio::GetInstance()->GetXAudio2(), &soundData);
	delete camera_;
	ModelManager::GetInstance()->Finalize();
}

void TitleScene::Update()
{


	// 2Dスプライトの更新
	for (size_t i = 0; i < sprites.size(); ++i) {
		Sprite* sprite = sprites[i];
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
		ImGui::Begin("Sprite");
		ImGui::DragFloat2("position", &position.x, 1.0f);
		ImGui::End();
		sprite->SetPosition(position);
	}

	// 3Dオブジェクトの更新
	for (int i = 0; i < object3ds.size(); ++i) {
		Object3d* obj = object3ds[i];
		obj->Update();
		Vector3 rotate = obj->GetRotation();
		if (i == 0) {
			rotate.y += 0.01f;
		}
		else if (i == 1) {
			rotate.z += 0.01f;
		}
		obj->SetRotation(rotate);
	}

	// デフォルトカメラの更新
	camera_->Update();
}

void TitleScene::Draw()
{
	///======================  描画準備  ========================///
	
	/// 3Dオブジェクトの描画準備
	Object3dCommon::Getinstance()->DrawPreference();
	/// 2DSpriteの描画準備
	SpriteCommon::Getinstance()->DrawPreference();

	///======================  実際に描画  ========================///

	// 2Dスプライト
	for (Sprite* sprite : sprites) {
		sprite->Draw();
	}
	// 3Dオブジェクト
	for (auto& obj : object3ds) {
		obj->Draw();
	}

}
