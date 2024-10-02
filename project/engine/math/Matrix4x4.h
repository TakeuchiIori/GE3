#pragma once
#include "Vector3.h"
#include <assert.h>
#include <cmath>
struct Matrix4x4 {
	float m[4][4];
};
struct UVTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
// 1. 行列の加法 
Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 2. 行列の減法 
Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 3. 行列の積 
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 4. 逆行列
Matrix4x4 Inverse(Matrix4x4 matrix);
// 5. 転置行列 
Matrix4x4 TransPose(Matrix4x4 matrix);
// 6. 単位行列 
Matrix4x4 MakeIdentity4x4();
// 7. 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 8. 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 9. 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// 10.回転行列
//    X軸回転行列
Matrix4x4 MakeRotateMatrixX(float radian);
//    Y軸回転行列
Matrix4x4 MakeRotateMatrixY(float radian);
//    Z軸回転行列
Matrix4x4 MakeRotateMatrixZ(float radian);

// 11. 3次元のアフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 12. レンタリングパイプラインVer2

//  透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float FovY, float aspectRatio, float nearClip, float farClip);
//  正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//  ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

