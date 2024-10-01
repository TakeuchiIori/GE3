#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = object3dCommon;
	// モデル読み込み
	modelData_ = LoadObjFile("Resources","axis.obj");

	VertexResource();

	IndexResource();

	MaterialResource();

	DirectionalLightResource();

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャ番号の取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	// Transform変数を作る
	transform_ = { {1.0f,1.0f,1.0f,},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };
	//AdjustTaxtureSize();
}

void Object3d::Update()
{
	CreateVertex();

	transform_ = { {1.0f,1.0f,1.0f,},{0.0f,rotation_,0.0f},{0.0f,3.0f,0.0f} };

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);

	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
	
}

void Object3d::Draw()
{
	// VertexBufferView
	object3dCommon_->GetDxCommon()->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// IndexBufferView
	//object3dCommon_->GetDxCommon()->GetcommandList()->IASetIndexBuffer(&indexBufferView_);//IBV

	// マテリアルCBufferの場所を指定
	object3dCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformatonMatrixCBuffferの場所を設定
	object3dCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	// SRVの設定
	object3dCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetsrvHandleGPU(modelData_.material.textureIndex)); // SRVのパラメータインデックスを変更
	// 平行光源のCBufferの場所を設定
	object3dCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 描画！！！DrawCall/ドローコール）
	object3dCommon_->GetDxCommon()->GetcommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
	
}


void Object3d::VertexResource()
{
	
	// リソース
	vertexResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

}

void Object3d::CreateVertex()
{
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	/*=====================================================//
							 index有
	=======================================================*/
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//// アンカーポイント
	//float left = 0.0f - anchorPoint_.x;
	//float right = 1.0f - anchorPoint_.x;
	//float top = 0.0f - anchorPoint_.y;
	//float bottom = 1.0f - anchorPoint_.y;

	//// 左右反転
	//if (isFlipX_) {
	//	left = -left;
	//	right = -right;
	//}
	////上下反転
	//if (isFlipY_) {
	//	top = -top;
	//	bottom = -bottom;
	//}

	//const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(modelData_.material.textureIndex);
	//float tex_left = textureLeftTop_.x / metadata.width;
	//float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	//float tex_top = textureLeftTop_.y / metadata.height;
	//float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;
	//// 左下
	//vertexData[0].position = { left, bottom, 0.0f, 1.0f };
	//vertexData[0].texcoord = { tex_left, tex_bottom };

	//// 左上
	//vertexData[1].position = { left, top, 0.0f, 1.0f };
	//vertexData[1].texcoord = { tex_left, tex_top };

	//// 右下
	//vertexData[2].position = { right, bottom, 0.0f, 1.0f };
	//vertexData[2].texcoord = { tex_right, tex_bottom };

	//// 右上
	//vertexData[3].position = { right, top, 0.0f, 1.0f };
	//vertexData[3].texcoord = { tex_right, tex_top };

	//// 2枚目の三角形用
	//vertexData[4].position = { left, top, 0.0f, 1.0f };  // 左上
	//vertexData[4].texcoord = { tex_left, tex_top };

	//vertexData[5].position = { right, bottom, 0.0f, 1.0f };  // 右下
	//vertexData[5].texcoord = { tex_right, tex_bottom };

	//vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	//vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	//vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	//vertexData[3].normal = { 0.0f,0.0f,-1.0f };
	//vertexData[4].normal = { 0.0f,0.0f,-1.0f };
	//vertexData[5].normal = { 0.0f,0.0f,-1.0f };
}

void Object3d::IndexResource()
{
	// リソース
	indexResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
	// リソースの先頭アドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースサイズはもとの頂点のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	CreateIndex();
}

void Object3d::CreateIndex()
{
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;  // 最初の三角形
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;  // 2つ目の三角形
	indexData[4] = 3;
	indexData[5] = 2;
}

void Object3d::MaterialResource()
{
	// リソース作成
	materialResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

	// リソース作成
	transformationMatrixResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込むためのアドレスを取得して割り当て
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 単位行列を書き込む
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();
}

void Object3d::DirectionalLightResource()
{
	directionalLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));

	// デフォルト値を設定
	directionalLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->direction = { 0.0f,-1.0f,0.0f };
	directionalLight_->intensity = 1.0f;

}

void Object3d::AdjustTaxtureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(modelData_.material.textureIndex);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;

}

Object3d::MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

Object3d::ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 必要な変数宣言
	ModelData modelData;		   // 構築するModelData
	vector<Vector4> positions;	   // 位置
	vector<Vector3> normals;	   // 法線
	vector<Vector2> texcoords;	   // テクスチャ座標
	string line;				   // ファイルから読んだ1行を格納

	// 2. ファイルを開く
	ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());						   // 開けなかったら止める

	while (getline(file, line)) {
		string identifier;
		istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未定。
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは　［位置/UV/法線］　で格納されているので分解してIndexを取得する
				istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					string index;
					getline(v, index, '/');				  // 区切りでインデックスうぃ読んでいく
					elementIndices[element] = stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順すろことで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}
