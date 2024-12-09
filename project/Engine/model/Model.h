#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include <optional>

// assimp
#include <assimp/scene.h>
#include <map>

class ModelCommon;
class Model
{
private: // 構造体

	// 頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct Color {
		float r, g, b;
	};

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

	struct Node {
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
		Node rootNode;
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
	/// アニメーション再生
	/// </summary>
	void PlayAnimation();

private:
	/// <summary>
	/// 頂点リソース
	/// </summary>
	void VertexResource();

	/// <summary>
	/// 頂点
	/// </summary>
	void CreateVertex();

	/// <summary>
	/// 任意の時刻を取得
	/// </summary>
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	/// <summary>
	/// 任意の時刻を取得
	/// </summary>
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
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
	/// アニメーション解析
	/// </summary>
	static Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	

public: // アクセッサ
	ModelData GetModelData() { return modelData_; }
	Matrix4x4 GetLocalMatrix() { return localMatrix_; }
private: // メンバ変数
	// ModelCommonのポインタ
	ModelCommon* modelCommon_;
	// objファイルのデータ
	ModelData modelData_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// バッファリソースの使い道を補足するバッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;


	// アニメーションの試運転用
	Animation animation_;

	Matrix4x4 localMatrix_;

	float animationTime = 0.0f;

};

