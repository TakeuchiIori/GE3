#include "ParticleManager.h"

// Engine
#include "DX./DirectXCommon.h"
#include "SrvManager./SrvManager.h"
#include "loaders./Texture./TextureManager.h"
#include "WinApp./WinApp.h"

// C++
#include <numbers>
#include <cassert>

#ifdef _DEBUG
#include "imgui.h"
#endif

// シングルトンインスタンスの初期化
std::unique_ptr<ParticleManager> ParticleManager::instance = nullptr;
std::once_flag ParticleManager::initInstanceFlag;

// ドロップダウンメニュー用の文字列
const char* blendModeNames[] = {
	"None",
	"Normal",
	"Add",
	"Subtract",
	"Multiply",
	"Screen"
};
ParticleManager* ParticleManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = std::make_unique<ParticleManager>();
		});
	return instance.get();
}

void ParticleManager::Finalize()
{
	instance.reset();
}

void ParticleManager::Initialize(SrvManager* srvManager)
{
	// ポインタを渡す
	this->dxCommon_ = DirectXCommon::GetInstance();
	this->srvManager_ = srvManager;

	accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField.area.min = { -10.0f,-10.0f,-10.0f };
	accelerationField.area.max = { 10.0f,10.0f,10.0f };

	// パイプライン生成
	CreateGraphicsPipeline();

	// 四角形の頂点を作成
	CreateVertexResource();

	// マテリアルリソース作成
	CreateMaterialResource();

	// ランダムエンジン初期化
	InitRandomEngine();
}

/// <summary>
/// パーティクルの更新処理
/// </summary>
void ParticleManager::Update()
{



	switch (currentUpdateMode_) {
	case kUpdateModeMove:
		UpdateParticleMove();

		break;
	case kUpdateModeRadial:
		UpdateParticlePlayer();

		break;
	case kUpdateModeSpiral:
		UpdateParticleSpiral();

		break;
	default:
		break;
	}
	// 行列の更新
	UpadateMatrix();
	// ブレンドモードの設定を反映
	Render(blendDesc_, currentBlendMode_);
#ifdef _DEBUG

	ShowUpdateModeDropdown();
#endif


}


void ParticleManager::Draw()
{


	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// すべてのパーティクルグループを描画
	for (auto& [groupName, particleGroup] : particleGroups_) {
		if (particleGroup.instance > 0) {
			// マテリアルCBufferの場所を指定
			dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			// SRVのDescriptorTableを設定
			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvIndex);
			// テクスチャのSRVのDescriptorTableを設定
			D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->GetsrvHandleGPU(particleGroup.materialData.textureFilePath);
			srvManager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureIndexSRV);
			// 描画
			dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), particleGroup.instance, 0, 0);

		}
	}


}


void ParticleManager::InitJson()
{
	jsonManager_ = new JsonManager("Particle", "Resources./JSON");
	
}

void ParticleManager::UpdateParticleMove()
{

	// カメラの行列を取得
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientHeight, 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix;
	Matrix4x4 viewProjectionMatrix;

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// ビュー・プロジェクション行列を生成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

	// パーティクルの更新処理
	for (auto& [groupName, particleGroup] : particleGroups_) {

		uint32_t numInstance = 0;

		for (auto particleIterator = particleGroup.particles.begin(); particleIterator != particleGroup.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				// パーティクルが生存時間を超えたら削除
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			if (numInstance >= kNumMaxInstance) {
				break;
			}
			if (IsCollision(accelerationField.area, (*particleIterator).transform.translate)) {
				(*particleIterator).velocity += accelerationField.acceleration * kDeltaTime;
			}
			// パーティクルの更新処理
			(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
			(*particleIterator).currentTime += kDeltaTime;
			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			scaleMatrix = ScaleMatrixFromVector3((*particleIterator).transform.scale);
			translateMatrix = TranslationMatrixFromVector3((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix{};
			if (useBillboard) {
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

			}
			else {
				worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, (*particleIterator).transform.rotate, (*particleIterator).transform.translate);
			}
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
			// インスタンシングデータの設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}
			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instance = numInstance;

		// GPU メモリにインスタンスデータを書き込む
		if (particleGroup.instancingData) {
			std::memcpy(particleGroup.instancingData, instancingData_, sizeof(ParticleForGPU) * numInstance);
		}
	}

}

void ParticleManager::UpdateParticlePlayer()
{

	// カメラの行列を取得
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientHeight, 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix;
	Matrix4x4 viewProjectionMatrix;

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// ビュー・プロジェクション行列を生成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

	// パーティクルの更新処理
	for (auto& [groupName, particleGroup] : particleGroups_) {

		uint32_t numInstance = 0;

		for (auto particleIterator = particleGroup.particles.begin(); particleIterator != particleGroup.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				// パーティクルが生存時間を超えたら削除
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}
			if (numInstance >= kNumMaxInstance) {
				break;
			}

			(*particleIterator).currentTime += kDeltaTime;
			// パーティクルの更新処理
			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			scaleMatrix = ScaleMatrixFromVector3((*particleIterator).transform.scale);
			translateMatrix = TranslationMatrixFromVector3((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix{};
			if (useBillboard) {
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

			}
			else {
				worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, (*particleIterator).transform.rotate, (*particleIterator).transform.translate);
			}
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
			// インスタンシングデータの設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}
			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instance = numInstance;

		// GPU メモリにインスタンスデータを書き込む
		if (particleGroup.instancingData) {
			std::memcpy(particleGroup.instancingData, instancingData_, sizeof(ParticleForGPU) * numInstance);
		}
	}


}

void ParticleManager::UpdateParticlePlayerWeapon(const Vector3& pos)
{
	// カメラの行列を取得
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientHeight, 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix;
	Matrix4x4 viewProjectionMatrix;

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// ビュー・プロジェクション行列を生成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

	for (auto& [groupName, particleGroup] : particleGroups_) {
		uint32_t numInstance = 0;

		for (auto particleIterator = particleGroup.particles.begin(); particleIterator != particleGroup.particles.end();) {
			// 寿命が尽きたパーティクルを削除
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			if (numInstance >= kNumMaxInstance) {
				break;
			}

			// パーティクルの動き：`pos` から放射状に発散
			Vector3 direction = (*particleIterator).transform.translate - pos; // `pos` を基準とした方向
			direction = Vector3::Normalize(direction) * 0.5f; // 放射状の速度調整
			(*particleIterator).velocity += direction;

			// 位置の更新
			(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;

			// 時間の更新
			(*particleIterator).currentTime += kDeltaTime;

			// アルファ値の更新（フェードアウト効果）
			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			scaleMatrix = ScaleMatrixFromVector3((*particleIterator).transform.scale);
			translateMatrix = TranslationMatrixFromVector3((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix = MakeAffineMatrix(
				(*particleIterator).transform.scale,
				(*particleIterator).transform.rotate,
				(*particleIterator).transform.translate
			);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

			// GPU用のインスタンシングデータ設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}
			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instance = numInstance;

		//// GPU メモリにインスタンスデータを書き込む
		//if (particleGroup.instancingData) {
		//	std::memcpy(particleGroup.instancingData, instancingData_, sizeof(ParticleForGPU) * numInstance);
		//}
	}
}

void ParticleManager::UpdateParticleRadial()
{
	// カメラの行列を取得
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientHeight, 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix;
	Matrix4x4 viewProjectionMatrix;

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// ビュー・プロジェクション行列を生成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	for (auto& [groupName, particleGroup] : particleGroups_) {
		uint32_t numInstance = 0;

		for (auto particleIterator = particleGroup.particles.begin(); particleIterator != particleGroup.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			if (numInstance >= kNumMaxInstance) {
				break;
			}

			// ランダム方向の正規化
			Vector3 randomDirection = {
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f
			};
			Vector3 normalizedDirection = Vector3::Normalize(randomDirection);

			// 加速度の大きさを計算
			float acceleration = Vector3::Length(accelerationField.acceleration);

			// 手動で x, y, z にスカラーを掛け算
			normalizedDirection.x *= acceleration * kDeltaTime;
			normalizedDirection.y *= acceleration * kDeltaTime;
			normalizedDirection.z *= acceleration * kDeltaTime;

			// 更新処理
			(*particleIterator).velocity.x += normalizedDirection.x;
			(*particleIterator).velocity.y += normalizedDirection.y;
			(*particleIterator).velocity.z += normalizedDirection.z;

			(*particleIterator).transform.translate.x += (*particleIterator).velocity.x * kDeltaTime;
			(*particleIterator).transform.translate.y += (*particleIterator).velocity.y * kDeltaTime;
			(*particleIterator).transform.translate.z += (*particleIterator).velocity.z * kDeltaTime;

			(*particleIterator).currentTime += kDeltaTime;

			// アルファ値の更新
			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			scaleMatrix = ScaleMatrixFromVector3((*particleIterator).transform.scale);
			translateMatrix = TranslationMatrixFromVector3((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix{};
			if (useBillboard) {
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

			}
			else {
				worldMatrix = MakeAffineMatrix((*particleIterator).transform.scale, (*particleIterator).transform.rotate, (*particleIterator).transform.translate);
			}
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
			// インスタンシングデータの設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}
			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instance = numInstance;

		//if (particleGroup.instancingData) {
		//	std::memcpy(particleGroup.instancingData, instancingData_, sizeof(ParticleForGPU) * numInstance);
		//}
	}
}

void ParticleManager::UpdateParticleSpiral()
{
	// カメラの行列を取得
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientHeight, 0.1f, 100.0f);
	Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix;
	Matrix4x4 viewProjectionMatrix;

	// ビルボード用の行列
	billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// ビュー・プロジェクション行列を生成
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

	for (auto& [groupName, particleGroup] : particleGroups_) {
		uint32_t numInstance = 0;

		for (auto particleIterator = particleGroup.particles.begin(); particleIterator != particleGroup.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particleGroup.particles.erase(particleIterator);
				continue;
			}

			if (numInstance >= kNumMaxInstance) {
				break;
			}

			// 時間に応じて回転角度を計算
			float angle = (*particleIterator).currentTime * 10.0f; // 角度は時間に比例
			float radius = 1.0f + (*particleIterator).currentTime * 2.0f; // 半径は徐々に広がる

			// 螺旋状の位置更新 (XZ 平面)
			(*particleIterator).transform.translate.x = radius * cos(angle);
			(*particleIterator).transform.translate.z = radius * sin(angle);
			(*particleIterator).transform.translate.y += (*particleIterator).velocity.y * kDeltaTime;

			(*particleIterator).currentTime += kDeltaTime;

			// アルファ値の更新 (フェードアウト)
			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			// ワールド行列の計算
			scaleMatrix = ScaleMatrixFromVector3((*particleIterator).transform.scale);
			translateMatrix = TranslationMatrixFromVector3((*particleIterator).transform.translate);
			Matrix4x4 worldMatrix = MakeAffineMatrix(
				(*particleIterator).transform.scale,
				(*particleIterator).transform.rotate,
				(*particleIterator).transform.translate
			);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

			// インスタンシングデータの設定
			if (numInstance < kNumMaxInstance) {
				instancingData_[numInstance].WVP = worldViewProjectionMatrix;
				instancingData_[numInstance].World = worldMatrix;
				instancingData_[numInstance].color = (*particleIterator).color;
				instancingData_[numInstance].color.w = alpha;
				++numInstance;
			}
			++particleIterator;
		}

		// インスタンス数の更新
		particleGroup.instance = numInstance;

		// GPU メモリにインスタンスデータを書き込む
		if (particleGroup.instancingData) {
			std::memcpy(particleGroup.instancingData, instancingData_, sizeof(ParticleForGPU) * numInstance);
		}
	}
}


void ParticleManager::UpadateMatrix()
{

	//// カメラの行列を取得
	//Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, WinApp::kClientWidth / WinApp::kClientWidth, 0.1f, 100.0f);
	//Matrix4x4 backToFrontMatrix = MakeRotateMatrixY(std::numbers::pi_v<float>);
	//// ビルボード用の行列
	//billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	//billboardMatrix.m[3][0] = 0.0f;
	//billboardMatrix.m[3][1] = 0.0f;
	//billboardMatrix.m[3][2] = 0.0f;

	//// ビュー・プロジェクション行列を生成
	//viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}

void ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// 1. RootSignatureの作成

	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるので配列。
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// PixelShaderで使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// VertexShaderで使う
	rootParameters_[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う
	descriptionRootSignature_.pParameters = rootParameters_;								// ルートパラメーター配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);					// 配列の長さ


	// 1. パーティクルのRootSignatureの作成
	descriptorRangeForInstancing_[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing_[0].NumDescriptors = 1;
	descriptorRangeForInstancing_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing_;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing_);


	// Samplerの設定
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;							// バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;						// 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
	staticSamplers_[0].ShaderRegister = 0;												// レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		DirectXCommon::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	// 2. InputLayoutの設定
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

	// 3. BlendDtateの設定
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = true;

	SetBlendMode(blendDesc_, blendMode_);
	currentBlendMode_ = kBlendModeAdd;  // 現在のブレンドモード
	// α値のブレンド
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasterrizerStateの設定
	// 裏面（時計回り）を表示しない  [カリング]
	rasterrizerDesc_.CullMode = D3D12_CULL_MODE_NONE;/* D3D12_CULL_MODE_*/
	// 三角形の中を塗りつぶす
	rasterrizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	// 4. Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Particle.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
	pixelShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Particle.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	//書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void ParticleManager::CreateGraphicsPipeline()
{
	// ルートシグネチャ
	CreateRootSignature();

	// パイプライン
	SetGraphicsPipeline();
}

void ParticleManager::CreateVertexResource()
{

	// インスタンス用のTransformationMatrixリソースを作る
	instancingResource_ = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// 四角形
	modelData_.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });

	// バッファビュー作成
	CreateVertexVBV();
}


void ParticleManager::CreateVertexVBV()
{
	//頂点リソース
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	// 頂点バッファービュー
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();// リソースデータの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());// 使用するリソースのサイズは1頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);// 1頂点のサイズ
	//　データ書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0, nullptr);
}

void ParticleManager::CreateMaterialResource()
{
	// リソース作成
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

}



void ParticleManager::InitRandomEngine()
{
	// ランダムエンジンの初期化
	randomEngine_ = std::mt19937(seedGenerator_());
}

void ParticleManager::SetGraphicsPipeline()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};//graphicsPipelineState_
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();					 // Rootsignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;					 // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };										 // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };											 // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;							 // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc_;   				 // RasterrizerState
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

ParticleManager::Particle ParticleManager::CreateParticle(std::mt19937& randomEngine, const Vector3& position)
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 1.0f);
	Particle particle;
	particle.transform.scale = { 2.0f, 2.0f, 2.0f };
	particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
	Vector3 randomTranslate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	particle.transform.translate = position /*+ randomTranslate*/;
	particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	particle.color = { distColor(randomEngine) , distColor(randomEngine) , distColor(randomEngine) , 1.0f };
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0.0f;
	return particle;
}


void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath)
{
	// 登録済みの名前かチェック
	if (particleGroups_.contains(name)) {
		// 登録済みの名前なら早期リターン
		return;
	}
	// グループを追加
	particleGroups_[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups_[name];

	// マテリアルデータにテクスチャファイルパスを設定
	particleGroup.materialData.textureFilePath = textureFilePath;
	// テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(particleGroup.materialData.textureFilePath);
	// マテリアルデータにテクスチャのSRVインデックスを記録
	particleGroup.materialData.textureIndexSRV = TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.materialData.textureFilePath);
	// Instancing用のリソースを生成
	particleGroup.instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	particleGroup.srvIndex = srvManager_->Allocate();
	// 書き込むためのアドレスを取得
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));


	srvManager_->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
	// インスタンス数を初期化
	particleGroup.instance = 0;


}
//void ParticleManager::Emit(const std::string& name, const Vector3& position, uint32_t count)
//{
//	// 登録済みのパーティクルグループ名かチェック
//	auto it = particleGroups_.find(name);
//	assert(it != particleGroups_.end());
//	// 指定されたパーティクルグループにパーティクルを追加
//	ParticleGroup& group = it->second;
//	// 各パーティクルを生成し追加
//	for (uint32_t i = 0; i < count; ++i) {
//		group.particles.emplace_back(CreateParticle(randomEngine_, position));
//		//group.particles.push_back(newParticle);
//	}
//}

std::list<ParticleManager::Particle> ParticleManager::Emit(const std::string& name, const Vector3& position, uint32_t count)
{
	// 登録済みのパーティクルグループ名かチェック
	auto it = particleGroups_.find(name);
	assert(it != particleGroups_.end());

	// 指定されたパーティクルグループにパーティクルを追加
	ParticleGroup& group = particleGroups_[name];

	// 生成したパーティクルを格納するリスト
	std::list<ParticleManager::Particle> emittedParticles;

	// 各パーティクルを生成し追加
	for (uint32_t i = 0; i < count; ++i) {
		Particle newParticle = CreateParticle(randomEngine_, position);
		
		emittedParticles.push_back(newParticle);
	}
	group.particles.splice(group.particles.end(), emittedParticles);
	return emittedParticles;
}
void ParticleManager::SetBlendMode(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode)
{
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		blendDesc.RenderTarget[i].BlendEnable = TRUE;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	switch (blendMode)
	{
	case kBlendModeNormal:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	case kBlendModeAdd:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultiply:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		break;
	case kBlendModeScreen:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		// 他のモードは処理なし
		 // kBlendModeNone や kBlendModeNormal の場合はデフォルト設定を使用
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendDesc.RenderTarget[i].BlendEnable = FALSE;
		}
		break;
	}
}

void ParticleManager::ShowBlendModeDropdown(BlendMode& currentBlendMode)
{
#ifdef _DEBUG
	ImGui::Begin("Particle");
	ImGui::Checkbox("useBillboard", &useBillboard);
	if (ImGui::BeginCombo("Blend Mode", blendModeNames[currentBlendMode]))
	{
		for (int i = 0; i < kCount0fBlendMode; ++i)
		{
			bool isSelected = (currentBlendMode == static_cast<BlendMode>(i));
			if (ImGui::Selectable(blendModeNames[i], isSelected))
			{
				currentBlendMode = static_cast<BlendMode>(i);
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
#endif // DEBUG
}

// メインループの一部として呼び出す
void ParticleManager::Render(D3D12_BLEND_DESC& blendDesc, BlendMode& currentBlendMode)
{
	//ShowBlendModeDropdown(currentBlendMode);

	static BlendMode lastBlendMode = kBlendModeNone;

	ShowBlendModeDropdown(currentBlendMode);

	if (currentBlendMode != lastBlendMode)
	{
		// ブレンドモードの変更を適用
		SetBlendMode(blendDesc, currentBlendMode);
		// PSOを再作成
		SetGraphicsPipeline();
		lastBlendMode = currentBlendMode;
	}

}
void ParticleManager::ShowUpdateModeDropdown()
{
#ifdef _DEBUG
	ImGui::Begin("Particle");

	const char* updateModeNames[] = {
		"Move",
		"Radial",
		"Spiral",
	};

	int currentMode = static_cast<int>(currentUpdateMode_);
	if (ImGui::Combo("Update Mode", &currentMode, updateModeNames, IM_ARRAYSIZE(updateModeNames))) {
		currentUpdateMode_ = static_cast<ParticleUpdateMode>(currentMode);
	}


	// 加速度の調整
	ImGui::Text("Acceleration");
	ImGui::DragFloat3("Acceleration", &accelerationField.acceleration.x, 0.1f, -100.0f, 100.0f);

	// 範囲 (AABB) の調整
	ImGui::Text("Area Min");
	ImGui::DragFloat3("Area Min", &accelerationField.area.min.x, 0.1f, -100.0f, 100.0f);

	ImGui::Text("Area Max");
	ImGui::DragFloat3("Area Max", &accelerationField.area.max.x, 0.1f, -100.0f, 100.0f);

	ImGui::End();
#endif
}
