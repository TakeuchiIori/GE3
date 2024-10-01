#pragma once
#include <string>
#include <vector>
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"

//　構造体
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};
//前方宣言
class Object3dCommon;
// 3Dオブジェクト
class Object3d
{
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Object3dCommon* object3dCommon);

private:
	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);


private: // メンバ変数
	Object3dCommon* object3dCommon_ = nullptr;
	// objファイルのデータ
	ModelData modelData_;
};

