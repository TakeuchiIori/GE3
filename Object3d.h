#pragma once
#include <string>
//前方宣言
class Object3dCommon;
// 3Dオブジェクト
class Object3d
{
public: //　構造体
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

