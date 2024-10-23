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

public: // カメラ
	/// <summary>
	/// 追従カメラ
	/// </summary>
	void FollowCamera(Vector3& target );

	/// <summary>
   /// 神視点カメラを設定する
   /// </summary>
	void SetTopDownCamera(const Vector3& position);


public: // アクセッサ
    // Setter
    void SetRotate(const Vector3& rotate) {transform_.rotate = rotate;}
    void SetTranslate(const Vector3& translate) {transform_.translate = translate; }
	void SetFovY(float fovY) { fovY_ = fovY; }
    void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio;}
	void SetNearClip(float nearClip) { nearClip_ = nearClip; }
    void SetFarClip(float farClip) {farClip_ = farClip;     }

	// getter
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	Vector3 GetRotate() const { return transform_.rotate; }
	Vector3 GetTranslate() const { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }

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

	Vector3 followCameraOffsetPosition_ = Vector3(0.0f, 30.0f, -50.0f);

	Vector3 followCameraOffsetRotare_ = Vector3(0.51f, 0.0f, 0.0f);

};

