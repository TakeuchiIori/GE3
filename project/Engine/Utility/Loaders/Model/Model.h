#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <optional>
#include <span>

// Engine
#include "DX./DirectXCommon.h"
#include "WorldTransform./WorldTransform.h"

// Math
#include "MathFunc.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"

// assimp
#include <assimp/scene.h>
#include <map>


class SrvManager;
class Line;
class ModelCommon;
class Model
{
public: // 構造体
	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct Color {
		float r, g, b;
	};
	// マテリアルデータ
	struct MaterialData {
		std::string name;
		float Ns;
		Color Ka;	// 環境光色
		Color Kd;	// 拡散反射色
		Color Ks;	// 鏡面反射光
		float Ni;
		float d;
		uint32_t illum;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};
	// ノード
	struct Node {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};
	// モデルデータ
	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
	};
	// インフルエンス
	const static uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointindices;
	};
	// マトリックスパレット
	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix;  // 位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
	};
	// スキンクラスター
	struct SkinCluster {
		std::vector<Matrix4x4> inverseBindposeMatrices;
		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	};

	struct KeyframeVector3 {
		Vector3 value;			 // キーフレームの値
		float time;				 // キーフレームの時刻（秒）
	};

	struct KeyframeQuaternion {
		Quaternion value;		 // キーフレームの値
		float time;				 // キーフレームの時刻（秒）
	};

	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};

	template<typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	struct NodeAnimation {
		std::vector<KeyframeVector3> translate;
		std::vector<KeyframeQuaternion> rotate;
		std::vector<KeyframeVector3> scale;
	};

	struct Animation {
		float duration; // アニメーション全体の尺（秒）
		// NodeAnimationの集合。Node名で開けるように
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	struct Joint {
		QuaternionTransform transform; // Transform情報
		Matrix4x4 localMatrix; // localMatrix
		Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
		std::string name; // 名前
		std::vector<int32_t> children; // 子JointのIndexのリスト。いなければ空
		int32_t index; // 自身のIndex
		std::optional<int32_t> parent; // 親JointのIndex。いなければnull
	};

	struct Skeleton {
		int32_t root; // RootJointのIndex
		std::map<std::string, int32_t> jointMap; // Joint名とIndexとの辞書
		std::vector<Joint> joints; // 所属しているジョイント
	};

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename ,bool isAnimation = false);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	//  スケルトンの描画　※DrawLineを調整中なので仮
	/// </summary>
	/// <param name="skeleton"></param>
	void DrawSkeleton(const Skeleton& skeleton, Line& line);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// アニメーション再生
	/// </summary>
	void PlayAnimation();

	/// <summary>
	/// スキンクラスターの更新
	/// </summary>
	void UpdateSkinCluster(SkinCluster& skinCluster,const Skeleton& skeleton);

private:
	/// <summary>
	/// 頂点リソース
	/// </summary>
	void CreateVertex();

	/// <summary>
	/// INdexリソース作成
	/// </summary>
	void CreteIndex();

	/// <summary>
	/// ジョイント作成
	/// </summary>
	/// <param name="node"></param>
	/// <param name="parent"></param>
	/// <param name="joints"></param>
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>
	/// スケルトン作成
	/// </summary>
	/// <param name="rootNode"></param>
	Skeleton CreateSkeleton(const Node& rootNode);

	/// <summary>
	/// スケルトンの更新
	/// </summary>
	void UpdateSkeleton(Skeleton& skeleton);

	/// <summary>
	/// アニメーション適用
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="animation"></param>
	/// <param name="animationTime"></param>
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

	/// <summary>
	/// 任意の時刻を取得
	/// </summary>
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	/// <summary>
	/// 任意の時刻を取得
	/// </summary>
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);


	SkinCluster CreateSkinCluster(const Skeleton& skeleton, const
		ModelData& modelData, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize);

private:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	static Node ReadNode(aiNode* node);

	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	
	/// <summary>
	/// .objファイルの読み取り
	/// </summary>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	
	/// <summary>
	/// .objファイルの読み取り　（assimp）
	/// </summary>
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// .objファイルの読み取り　（Index）
	/// </summary>
	static ModelData LoadModelIndexFile(const std::string& directoryPath, const std::string& filename);



	/// <summary>
	/// アニメーション解析
	/// </summary>
	static Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	

public: // アクセッサ
	ModelData GetModelData() { return modelData_; }
	Matrix4x4 GetLocalMatrix() { return localMatrix_; }
	Skeleton GetSkeleton() { return skeleton_; }
private: // メンバ変数
	// 外部からのポインタ
	ModelCommon* modelCommon_;
	// objファイルのデータ
	ModelData modelData_;

	// 頂点リソースなど
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;

	// Indexリソースなど
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* mappedIndex_ = nullptr;

	// アニメーションの試運転用
	Animation animation_;
	Matrix4x4 localMatrix_;
	float animationTime_ = 0.0f;

	Skeleton skeleton_;

private: // Skinning

	SrvManager* srvManager_ = nullptr;

	SkinCluster* skinCluster_ = nullptr;
};

