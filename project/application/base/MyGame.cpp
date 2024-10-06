#include "MyGame.h"

void MyGame::Initialize()
{
	
	winApp_ = new WinApp();
	winApp_->Initialize();

	
	input_ = new Input();
	input_->Initialize(winApp_);

	
	
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);


	audio_ = new Audio();
	audio_->Initialize();


#pragma region 基礎システムの初期化

	// SRVマネージャの初期化
	
	srvManager_ = new SrvManager();
	srvManager_->Initialize(dxCommon_);

	
	// スプライト共通部の初期化
	spriteCommon_ = new SpriteCommon();
	spriteCommon_->Initialize(dxCommon_);

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	// 3Dオブジェクト共通部の初期化
	
	object3dCommon_ = new Object3dCommon();
	object3dCommon_->Initialize(dxCommon_);

	// 3Dモデルマネージャの初期化
	ModelManager::GetInstance()->Initialze(dxCommon_);
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	// デフォルトカメラ
	camera_ = new Camera();
	camera_->SetRotate({ 0.0f,0.0f,0.0f });
	camera_->SetTranslate({ 0.0f,4.0f,-10.0f });
	object3dCommon_->SetDefaultCamera(camera_);



#pragma endregion 基礎システムの初期化



#pragma region 最初のシーンの初期化

	// スプライトの初期化

	std::string textureFilePath[2] = { "Resources./monsterBall.png" ,"Resources./uvChecker.png" };
	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon_, textureFilePath[1]);
		// 移動テスト: インデックスに応じてX、Y座標をずらして配置
		Vector2 position;
		position.x = i * 200.0f;
		position.y = 0.0f;
		sprite->SetPosition(position);
		sprite->SetSrvManager(srvManager_);

		// 初期色の設定（任意で設定）
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色
		sprite->SetColor(color);
		if (i % 2 != 0) {
			sprite->ChangeTexture(textureFilePath[0]);
		}
		else {
			sprite->ChangeTexture(textureFilePath[1]);
		}
		sprites.push_back(sprite);
	}



	// 複数のオブジェクトを初期化
	uint32_t numObjects = 2;
	for (uint32_t i = 0; i < numObjects; ++i) {
		Object3d* object = new Object3d();
		object->Initialize(object3dCommon_);
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

	soundData = audio_->LoadWave("Resources./fanfare.wav");
}

void MyGame::Finalize()
{
	// 各解放の処理
	CloseHandle(dxCommon_->GetfenceEvent());
	delete input_;
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;
	//delete imguiManager_;
	delete dxCommon_;
	delete spriteCommon_;
	delete object3dCommon_;
	delete camera_;
	delete srvManager_;
	// 描画処理
	for (auto& obj : object3ds) {
		delete obj;
	}
	// 描画
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	// テクスチャマネージャの終了
	TextureManager::GetInstance()->Finalize();
	// 3Dモデルマネージャの終了
	ModelManager::GetInstance()->Finalize();
	audio_->SoundUnload(audio_->GetXAudio2(), &soundData);
}

void MyGame::Update()
{


	// キーボード入力
	input_->Update(winApp_);

	/*================================================================//
							   ゲームの処理開始
	//================================================================*/

	// 2Dスプライトの更新
	for (size_t i = 0; i < sprites.size(); ++i) {
		Sprite* sprite = sprites[i];
		sprite->Update();

		// 回転テスト
		float rotation = sprite->GetRotation();
		rotation += 0.01f;
		//sprite->SetRotation(rotation);

		// サイズ変化
		Vector2 size = sprite->GetSize();
		size.x += 0.01f;  // サイズ変化を少し小さくする
		size.y += 0.01f;  // サイズ変化を少し小さくする
		//sprite->SetSize(size);

		// 色テスト
		Vector4 color = sprite->GetColor();
		color.x += 0.01f;
		if (color.x > 1.0f) {
			color.x -= 1.0f;
		}
		//sprite->SetColor(color);

		Vector2 position = sprite->GetPosition();

		//ImGui::Begin("Sprite");
		//ImGui::DragFloat2("position", &position.x, 1.0f);
		//ImGui::End();
		sprite->SetPosition(position);
	}

	// 3Dオブジェクトの更新
	for (int i = 0; i < object3ds.size(); ++i) {
		Object3d* obj = object3ds[i];
		obj->Update();
		Vector3 rotate = obj->GetRotation();
		if (i == 0) {
			// 1つ目のオブジェクト: Y軸で回転
			rotate.y += 0.01f;
		}
		else if (i == 1) {
			// 2つ目のオブジェクト: Z軸で回転
			rotate.z += 0.01f;
		}
		// 更新した回転、スケール、位置を適用
		obj->SetRotation(rotate);
	}

	// デフォルトカメラの更新
	camera_->Update();

}

void MyGame::Draw()
{
	// Srvの描画準備
	srvManager_->PreDraw();
	// DirectXの描画準備。全ての描画にグラフィックスコマンドを積む
	dxCommon_->PreDraw();


	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
	object3dCommon_->DrawPreference();

	// 2DSpriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	spriteCommon_->DrawPreference();


	// 描画
	// 2Dスプライト
	for (Sprite* sprite : sprites) {
		sprite->Draw();
	}
	// 3Dオブジェクト
	for (auto& obj : object3ds) {
		obj->Draw();
	}

	// ImGui描画
	//imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
