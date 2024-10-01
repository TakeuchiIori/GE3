#pragma once
#include <string>
#include <vector>
#include "math/Vector4.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
//前方宣言
class Object3dCommon;
// 3Dオブジェクト
class Object3d
{
public: //　構造体
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

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Object3dCommon* object3dCommon);

private:



private: // メンバ変数
	Object3dCommon* object3dCommon_ = nullptr;
};

