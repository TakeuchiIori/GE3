#include "Model.h"
#include "ModelCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "TextureManager.h"

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename)
{
	// 引数から受け取ってメンバ変数に記録する
	modelCommon_ = modelCommon;

	// モデル読み込み
	//modelData_ = LoadObjFile(directorypath, filename);

	modelData_ = LoadModelFile(directorypath, filename);
	// 頂点データの初期化
	VertexResource();

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	// 読み込んだテクスチャ番号の取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);



	// マテリアルの初期化
	//MaterialResource();

}

void Model::Draw()
{
	// VertexBufferView
	modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	// SRVの設定
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetsrvHandleGPU(modelData_.material.textureFilePath)); // SRVのパラメータインデックスを変更
	// 描画！！！DrawCall/ドローコール）
	modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

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

	CreateVertex();
}

void Model::CreateVertex()
{
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点を作成
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}


Model::Node Model::ReadNode(aiNode* node) {
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;

	// スケール、回転、平行移動を分解
	aiVector3D scaling;
	aiQuaternion rotation;
	aiVector3D position;
	aiLocalMatrix.Decompose(scaling, rotation, position);

	// 回転を適用
	aiMatrix4x4 rotationMatrix = aiMatrix4x4(rotation.GetMatrix());
	rotationMatrix.Transpose(); // 行列形式変換

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.localMatrix.m[row][col] = rotationMatrix[row][col];
		}
	}

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
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

		if (identifier == "newmtl") {
			s >> materialData.name;
		}
		else if (identifier == "Ns") {
			s >> materialData.Ns;
		}
		else if (identifier == "Ka") {
			s >> materialData.Ka.r >> materialData.Ka.g >> materialData.Ka.b;
		}
		else if (identifier == "Kd") {
			s >> materialData.Kd.r >> materialData.Kd.g >> materialData.Kd.b;
		}
		else if (identifier == "Ks") {
			s >> materialData.Ks.r >> materialData.Ks.g >> materialData.Ks.b;
		}
		else if (identifier == "Ni") {
			s >> materialData.Ni;
		}
		else if (identifier == "d") {
			s >> materialData.d;
		}
		else if (identifier == "illum") {
			s >> materialData.illum;
		}
		else if (identifier == "map_Kd") {
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

// assimp
Model::ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	//=================================================//
	//					 .obj読み込み
	//=================================================//
	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュが無いと非対応
	modelData.rootNode = ReadNode(scene->mRootNode);
	//=================================================//
	//					 Meshを解析
	//=================================================//

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());			  // 法線が無い場合のMeshは 04-00 では非対応
		assert(mesh->HasTextureCoords(0));	  // TexcoordがないMeshは今回は非対応
		

		//=================================================//
		//				  Meshの中身（Face）を解析
		//=================================================//

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);		   // 三角形のみ

			//=================================================//
			//				  Faceの中身（Vertex）を解析
			//=================================================//

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				texcoord.y = 1.0f - texcoord.y;
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z ,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				// aiProcess_MakeLeftHandedは z *= -1 で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}
		}
	}
	//=================================================//
	//				  materialを解析
	//=================================================//

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}

	}
	
	return modelData;
}
