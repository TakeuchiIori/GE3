#include "Model.h"
#include "ModelCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
void Model::Initialize(ModelCommon* modelCommon)
{
	// 引数から受け取ってメンバ変数に記録する
	modelCommon_ = modelCommon;

	// モデル読み込み
	modelData_ = LoadObjFile("Resources", "axis.obj");

	// 頂点データの初期化
	VertexResource();

	// マテリアルの初期化
	MaterialResource();

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	// 読み込んだテクスチャ番号の取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
	
}

void Model::Update()
{


}

void Model::Draw()
{
	// VertexBufferView
	modelCommon_->GetDxCommon()->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// マテリアルCBufferの場所を指定
	modelCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// SRVの設定
	modelCommon_->GetDxCommon()->GetcommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetsrvHandleGPU(modelData_.material.textureIndex)); // SRVのパラメータインデックスを変更
	// 描画！！！DrawCall/ドローコール）
	modelCommon_->GetDxCommon()->GetcommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

void Model::VertexResource()
{
	// リソース
	vertexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Model::CreateVertex()
{
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::MaterialResource()
{
	// リソース作成
	materialResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

	// リソース作成
	//transformationMatrixResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込むためのアドレスを取得して割り当て
	//transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 単位行列を書き込む
	//transformationMatrixData_->WVP = MakeIdentity4x4();
	//transformationMatrixData_->World = MakeIdentity4x4();
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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

Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
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
