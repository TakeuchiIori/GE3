#include "Spline.h"
#include "ModelManager.h"

void Spline::Initialize()
{
    // .obj読み込み
    ModelManager::GetInstance()->LoadModel("player.obj");

    obj_ = std::make_unique<Object3d>();
    obj_->Initialize();
    obj_->SetModel("player.obj");

    worldTransform_.Initialize();

    controlPoints_ = {
        {0,0,0},
        {10,10,10},
        {10,15,0},
        {20,15,0},
        {20,0,0},
        {30,0,0},
    };

}

void Spline::Update()
{
    worldTransform_.UpdateMatrix();
}

void Spline::Draw()
{ // もし描画用頂点リストが空なら、初期化して生成
    if (pointsDrawing_.empty()) {
        pointsDrawing_ = GenerateCatmullRomSplinePoints(controlPoints_, segmentCount);
    }

    // ワールド変換を適用して各点を描画
    for (size_t i = 0; i < pointsDrawing_.size() - 1; ++i) {
        worldTransform_.translation_ = pointsDrawing_[i];
        // Object3dを使用して描画
        obj_->Draw(worldTransform_);
    }
}
