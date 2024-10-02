#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
class Camera
{
public: // メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public: // アクセッサ
    // Setter
    void SetRotate(const Vector3& rotate) {transform_.rotate = rotate;}
    void SetTranslate(const Vector3& translate) {transform_.translate = translate; }
	void SetFovY(float fovY) { fovY_ = fovY; }
    void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio;}
	void SetNearClip(float nearClip) { nearClip_ = nearClip; }
    void SetFarClip(float farClip) {farClip_ = farClip;     }

	// getter
	const Matrix4x4& GetWorldMatrix() const { worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { viewProjectionMatrix_; }
	Vector3 GetRotate() const { transform_.rotate; }
	Vector3 GetTranslate() const { transform_.translate; }
	Vector3 GetScale() const { transform_.scale; }

private: // メンバ変数
	Transform transform_;	   
	Matrix4x4 worldMatrix_;	   
	Matrix4x4 viewMatrix_;	   
	Matrix4x4 projectionMatrix_;  // 投影行列
	Matrix4x4 viewProjectionMatrix_;
	float fovY_;        // 水平方向視野角 (Field of View)
	float aspectRatio_;          // アスペクト比
	float nearClip_;			 // ニアクリップ距離
	float farClip_;				 // ファークリップ距離

};

