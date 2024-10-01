#include <Windows.h>
#include <string>
#include <format>
#include <cstdint>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "VertexData.h"
#include "math/Vector4.h"
#include "math/Matrix4x4.h"


#include "externals/imgui/imgui_impl_dx12.h"

#include "externals/DirectXTex/DirectXTex.h"
#include "LoadTexture.h"
#include "CreateTextureResource.h"


#include "DescriptorHandle.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "math/MathFunc.h"
#include "TransformationMatrix.h"

#include <corecrt_math_defines.h>

#include<wrl.h>
#include "ResourceObject.h"

#include "D3DResorceLeakChecker.h"

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "externals/imgui/imgui_impl_win32.h"



#pragma comment(lib,"dxguid.lib")

#pragma comment(lib,"dxcompiler.lib")

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResorceLeakChecker* leakCheack;
	WinApp* winApp_ = nullptr;
	winApp_ = new WinApp();
	winApp_->Initialize();

	
	Input* input_ = nullptr;
	input_ = new Input();
	input_->Initialize(winApp_);

	DirectXCommon* dxCommon_ = nullptr;
	// DirectXの初期化
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);

#pragma region 基礎システムの初期化

	SpriteCommon* spriteCommon_ = nullptr;
	// スプライト共通部の初期化
	spriteCommon_ = new SpriteCommon();
	spriteCommon_->Initialize(dxCommon_);

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon_);

	// 3Dオブジェクト共通部の初期化
	Object3dCommon* object3dCommon_ = nullptr;
	object3dCommon_ = new Object3dCommon();
	object3dCommon_->Initialize(dxCommon_);

#pragma endregion 基礎システムの初期化

	

#pragma region 最初のシーンの初期化

	// スプライトの初期化
	std::vector<Sprite*> sprites;
	std::string textureFilePath[2] = { "Resources./monsterBall.png" ,"Resources./uvChecker.png" };
	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon_, textureFilePath[0]);
		// 移動テスト: インデックスに応じてX、Y座標をずらして配置
		Vector2 position;
		position.x = i * 200;
		position.y = 0.0f;
		sprite->SetPosition(position);

		// 初期色の設定（任意で設定）
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色
		sprite->SetColor(color);
		if (i % 2 == 0 ) {
			sprite->ChangeTexture(textureFilePath[0]);
		}
		else {
			sprite->ChangeTexture(textureFilePath[1]);
		}
		sprites.push_back(sprite);
	}

	// 3Dオブジェクトの初期化
	Object3d* object3d = new Object3d();
	object3d->Initialize(object3dCommon_);

#pragma endregion 最初のシーンの終了


	
	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		// windowにメッセージが来てたら最優先で処理させる
		if (winApp_->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		
			input_->Update(winApp_);

			// 更新
			for (size_t i = 0; i < sprites.size(); ++i) {
				Sprite* sprite = sprites[i];
				sprite->Update();

				// 回転テスト
				float rotation = sprite->GetRotation();
				rotation += 0.01f;
				sprite->SetRotation(rotation);

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
				sprite->SetColor(color);
			}

			// DirectXの描画準備。全ての描画にグラフィックスコマンドを積む
			dxCommon_->PreDraw();

			// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
			object3dCommon_->DrawPreference();

			// 2DSpriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
			spriteCommon_->DrawPreference();

	
			// 描画
			for (Sprite* sprite : sprites) {
				sprite->Draw();
			}
		
			
			// DirectXの描画終了
			dxCommon_->PostDraw();
			


	}// ゲームループの終了

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// 各解放の処理
	CloseHandle(dxCommon_->GetfenceEvent());
	delete input_;
	winApp_->Finalize();
	delete winApp_;
	winApp_ = nullptr;
	delete dxCommon_;
	delete spriteCommon_;
	delete object3d;
	delete object3dCommon_;
	// 描画
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	// テクスチャマネージャの終了
	TextureManager::GetInstance()->Finalize();
	
	
	return 0;// main関数のリターン
}

