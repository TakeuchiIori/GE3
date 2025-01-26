
// Engine
#include "Model.h"
#include "ModelCommon.h"
#include "SrvManager/SrvManager.h"
#include "Loaders./Texture./TextureManager.h"
#include "Drawer./LineManager/Line.h"

// C++
#include <assert.h>
#include <fstream>
#include <sstream>


// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, bool isAnimation)
{
	isAnimation_ = isAnimation;
	// 引数から受け取ってメンバ変数に記録する
	modelCommon_ = modelCommon;

	srvManager_ = SrvManager::GetInstance();

	// モデル読み込み
	modelData_ = LoadModelIndexFile(directorypath, filename);

	// アニメーションをするならtrue
	if (isAnimation_) {
		animation_ = LoadAnimationFile(directorypath, filename);


		if (modelData_.hasBones) {
			// 骨の作成
			skeleton_ = CreateSkeleton(modelData_.rootNode);

			skinCluster_ = CreateSkinCluster(skeleton_, modelData_);
		}

	}

	// 頂点データの初期化
	CreateVertex();

	// 頂点データを参照してIndexデータの作成
	CreteIndex();

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	// 読み込んだテクスチャ番号の取得
	modelData_.material.textureIndex =TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

}

void Model::Draw()
{
	if (skeleton_.joints.empty()) {
		// スケルトンが存在しない場合
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	}
	else {
		// スケルトンが存在する場合
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView_,                       // VertexDataのVBV
			skinCluster_.influenceBufferView        // InfluenceのVBV
		};
		modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs); // VBVを設定

		// スケルトン用のSRVを設定
		modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(7, skinCluster_.paletteSrvHandle.second);
	}


	// indexbufferView
	modelCommon_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView_); // IBVを設定
	// SRVの設定
	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetsrvHandleGPU(modelData_.material.textureFilePath)); // SRVのパラメータインデックスを変更
	// 描画！！！DrawCall/ドローコール）
	modelCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(modelData_.indices.size()), 1, 0, 0,0);

}


std::vector<Vector3> Model::GetConnectionPositions()
{
	std::vector<Vector3> connectionPositions;

	for (const auto& joint : skeleton_.joints) {
		if (joint.parent) { // 親が存在するボーンのみ
			// 親の位置
			Vector3 parentPosition = {
				skeleton_.joints[*joint.parent].skeletonSpaceMatrix.m[3][0],
				skeleton_.joints[*joint.parent].skeletonSpaceMatrix.m[3][1],
				skeleton_.joints[*joint.parent].skeletonSpaceMatrix.m[3][2]
			};

			// 現在のボーンの位置
			Vector3 currentPosition = {
				joint.skeletonSpaceMatrix.m[3][0],
				joint.skeletonSpaceMatrix.m[3][1],
				joint.skeletonSpaceMatrix.m[3][2]
			};

			// 中間位置（オプション：つなぎ目として描画する場合）
			Vector3 connectionPosition = (parentPosition + currentPosition) * 0.5f;

			connectionPositions.push_back(connectionPosition);
		}
	}

	return connectionPositions;
}

uint32_t Model::GetConnectionCount()
{
	return static_cast<int>(skeleton_.joints.size()) - 1; // 全ボーン数 - ルートボーン
}



Vector3 Model::ExtractJointPosition(const Joint& joint) const
{
	return {
		joint.skeletonSpaceMatrix.m[3][0],
		joint.skeletonSpaceMatrix.m[3][1],
		joint.skeletonSpaceMatrix.m[3][2]
	};
}


void Model::DrawSkeletonRecursive(const Skeleton& skeleton, Line& line, int32_t currentIndex)
{
	const Joint& currentJoint = skeleton.joints[currentIndex];

	// 現在のジョイントの位置を取得
	Vector3 currentPosition = {
		currentJoint.skeletonSpaceMatrix.m[3][0],
		currentJoint.skeletonSpaceMatrix.m[3][1],
		currentJoint.skeletonSpaceMatrix.m[3][2]
	};

	// 親が存在する場合、親ジョイントと現在のジョイントを線でつなぐ
	if (currentJoint.parent.has_value()) {
		const Joint& parentJoint = skeleton.joints[currentJoint.parent.value()];

		Vector3 parentPosition = {
			parentJoint.skeletonSpaceMatrix.m[3][0],
			parentJoint.skeletonSpaceMatrix.m[3][1],
			parentJoint.skeletonSpaceMatrix.m[3][2]
		};

		// 親ジョイントと現在のジョイントを線で描画
		//line.DrawLine(parentPosition, currentPosition);
	}

	// 子ジョイントを再帰的に処理する
	for (int32_t childIndex : currentJoint.children) {
		DrawSkeletonRecursive(skeleton, line, childIndex);
	}
}


void Model::DrawSkeleton(const Skeleton& skeleton,Line& line) {
	// スケルトンが空の場合は終了
	// ラインを描画
	//line.ClearVertices();
	if (skeleton.joints.empty()) {
		return;
	}

	// スケルトン内の全ての接続を描画
	for (const auto& connection : skeleton.connections) {
		int32_t parentIndex = connection.first;
		int32_t childIndex = connection.second;

		// 親ジョイントと子ジョイントのワールド座標を取得
		const Vector3& parentPosition = ExtractJointPosition(skeleton.joints[parentIndex]);
		const Vector3& childPosition = ExtractJointPosition(skeleton.joints[childIndex]);

		
		line.UpdateVertices(parentPosition, childPosition);
	}
}


void Model::UpdateAnimation()
{
	animationTime_ += 1.0f / 60.0f;
	animationTime_ = std::fmod(animationTime_, animation_.duration);
	if (modelData_.hasBones) {
		ApplyAnimation(skeleton_, animation_, animationTime_);
		UpdateSkeleton(skeleton_);
		UpdateSkinCluster(skinCluster_, skeleton_);
	}
	else {
		PlayAnimation(animationTime_);
	}
}


void Model::PlayAnimation(float animationTime)
{
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name]; // rootNodeのAnimationを取得
	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime); // 指定時刻の値を取得
	Quaternion rotate= CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);

	modelData_.rootNode.localMatrix = MakeAffineMatrix(scale, rotate, translate);
}

void Model::UpdateSkeleton(Skeleton& skeleton)
{
	// すべてのJointを更新。親が若いので通常ループで処理が可能になっている
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		if (joint.parent) { // 親がいれば親の行列を掛ける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;

		}
	}
}

void Model::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。
		// 下記のif文はC++17から可能になったinit-statement付きのif文。
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			
		}
	}
}



void Model::UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton)
{
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindposeMatrices.size());
		
		// スケルトン空間行列の計算
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindposeMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		
		// 法線用の行列を計算（転置逆行列）
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			TransPose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void Model::CreateVertex()
{
	// リソース
	vertexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// バッファ作成とデータ書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0, nullptr);
}

void Model::CreteIndex()
{
	indexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * modelData_.indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * static_cast<UINT>(modelData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0, nullptr);
}

int32_t Model::CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); // 現在登録されているIndexに
	joint.parent = parent;
	joints.push_back(joint); // SkeletonのJoint列に追加

	for (const Node& child : node.children) {
		// 子Jointを作成し、そのIndex
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自身のIndexを返す
	return joint.index;
}

Model::Skeleton Model::CreateSkeleton(const Model::Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とindexのマッピングを行いアクセスしやすくなる
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);

		if (joint.parent.has_value()) {
			skeleton.connections.emplace_back(joint.parent.value(), joint.index);
		}
	}

	return skeleton;
}


Model::SkinCluster Model::CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData)
{
	SkinCluster skinCluster;
	//auto device = modelCommon_->GetDxCommon()->GetDevice();
	//=========================================================//
	//					palette用のResourceを確保				   //
	//=========================================================//

	skinCluster.paletteResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() }; // spanを使ってアクセスできるようにする
	skinCluster.srvIndex = srvManager_->Allocate();
	skinCluster.paletteSrvHandle.first = srvManager_->GetCPUSRVDescriptorHandle(skinCluster.srvIndex);
	skinCluster.paletteSrvHandle.second = srvManager_->GetGPUSRVDescriptorHandle(skinCluster.srvIndex);
	srvManager_->CreateSRVforStructuredBuffer(skinCluster.srvIndex, skinCluster.paletteResource.Get(),
		UINT(skeleton.joints.size()), sizeof(WellForGPU));

	//=========================================================//
	//					Influece用のResourceを生成			   //
	//=========================================================//

	skinCluster.influenceResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size()); // 0埋め。weightを0にしておく
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };
	/// Influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);
	/// InverseBindPosematrixを格納する場所を作成して、単位行列で埋める
	skinCluster.inverseBindposeMatrices.resize(skeleton.joints.size());
	std::generate(skinCluster.inverseBindposeMatrices.begin(), skinCluster.inverseBindposeMatrices.end(), []() { return MakeIdentity4x4(); });

	//=========================================================//
	//			ModelDataを解析してInfluenceを埋める			   //
	//=========================================================//

	for (const auto& jointWeight : modelData.skinClusterData) { // ModelのSkinCluster情報を解析
		auto it = skeleton.jointMap.find(jointWeight.first); // jointweight.firstはjoint名なので、skeletonに対象となるjointは含まれるか判断
		if (it == skeleton.jointMap.end()) { // そんな名前は存在しない。なので次に回す
			continue;
		}
		// (*it).secondにはjointのindexが入ってるので、該当のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindposeMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex]; // 該当のいvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				if (currentInfluence.weights[index] == 0.0f) { // weight == 0が開いてる状態なので、その場所でweightとjointのindexを代入
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointindices[index] = (*it).second;
					break;
				}
			}
		}

	}	


	return skinCluster;
}

//================================




Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるか判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は一番後の時刻よりも後ろなので最後の値を返すことになる
	return (*keyframes.rbegin()).value;
}

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}


Model::Node Model::ReadNode(aiNode* node) {
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x,scale.y,scale.z }; // scaleはそのまま
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w }; // x軸を反転。さらに回転方向が逆なので軸を反転させる
	result.transform.translate = { -translate.x,translate.y,translate.z }; // x軸を反転
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

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

Model::ModelData Model::LoadModelIndexFile(const std::string& directoryPath, const std::string& filename)
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
	// ボーンが含まれているかを判別
	modelData.hasBones = HasBones(scene);
	//=================================================//
	//					 Meshを解析
	//=================================================//

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());			  // 法線が無い場合のMeshは 04-00 では非対応
		assert(mesh->HasTextureCoords(0));	  // TexcoordがないMeshは今回は非対応
		modelData.vertices.resize(mesh->mNumVertices); // 最初に頂点数分のメモリを確保しておく
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoords = mesh->mTextureCoords[0][vertexIndex];
			// 右手系->左手系への変換を忘れずに
			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoords.x, texcoords.y };
		}

		// 面からIndexの情報を取得する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			// 四角形だったら三角形に変換
			if (face.mNumIndices == 4) {
				uint32_t i0 = face.mIndices[0];
				uint32_t i1 = face.mIndices[1];
				uint32_t i2 = face.mIndices[2];
				uint32_t i3 = face.mIndices[3];

				// 四角形を2つの三角形に分割
				modelData.indices.push_back(i0);
				modelData.indices.push_back(i1);
				modelData.indices.push_back(i2);

				modelData.indices.push_back(i0);
				modelData.indices.push_back(i2);
				modelData.indices.push_back(i3);
			}
			// 三角形だったらそのまま使う
			else if (face.mNumIndices == 3) {
				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					modelData.indices.push_back(face.mIndices[element]);
				}
			}
		}

		//=================================================//
		//			 SkinCluster構築用のデータ取得を追加	   //
		//=================================================//
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];


			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindposeMatrix = MakeAffineMatrix(
				{ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindposeMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
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


Model::InterpolationType Model::MapAssimpBehaviourToInterpolation(aiAnimBehaviour preState, aiAnimBehaviour postState)
{
	if (preState == aiAnimBehaviour_CONSTANT || postState == aiAnimBehaviour_CONSTANT) {
		return InterpolationType::Step;
	}
	else if (preState == aiAnimBehaviour_LINEAR || postState == aiAnimBehaviour_LINEAR) {
		return InterpolationType::Linear;
	}
	else if (preState == aiAnimBehaviour_REPEAT || postState == aiAnimBehaviour_REPEAT) {
		return InterpolationType::CubicSpline;
	}

	return InterpolationType::Linear; // デフォルト
}

Model::Animation Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Animation animation; // 今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0); // アニメーション無し
	aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用　※複数対応必須
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

	// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		///// デフォルトの補間タイプをLinearに設定
		//nodeAnimation.interpolationType = InterpolationType::Linear;

		/// ------
		/// 補間の種類を取得
		/// ------
		nodeAnimation.interpolationType =
		MapAssimpBehaviourToInterpolation(nodeAnimationAssimp->mPreState, nodeAnimationAssimp->mPostState);




		// Position
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		// Scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}

		// Rotate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y , -keyAssimp.mValue.z ,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

	}

	return animation;
}



bool Model::HasBones(const aiScene* scene)
{
	// メッシュがなければボーンもない
	if (!scene->HasMeshes()) {
		return false;
	}

	// 各メッシュをチェック
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// ボーン数が0でなければボーンがあると判断
		if (mesh->mNumBones > 0) {
			return true;
		}
	}

	// どのメッシュにもボーンが含まれていない場合
	return false;
}
