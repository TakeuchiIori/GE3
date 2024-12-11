#pragma once

// Math
#include "Vector3.h"
#include "Matrix4x4.h"
#include "MathFunc.h"

class Camera;
class DirectXCommon;
class Line
{

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 線を描画
	/// </summary>
	void DrawLine();

private:

	/// <summary>
	/// 頂点を作成
	/// </summary>
	void CreateVertices();

public:

	void SetCamera(Camera* camera) { this->camera_ = camera; }

private:

	// 外部からのポインタ
	Camera* camera_ = nullptr;


	

};

